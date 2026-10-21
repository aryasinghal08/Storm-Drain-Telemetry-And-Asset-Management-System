#include "server.h"
#include "WeatherApiClient.h"
#include "Graph.h"
#include "HashTable.h"
#include "CircularQueue.h"
#include "MaxHeap.h"
#include "MaintenanceLog.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iomanip>
#include <map>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using Socket = SOCKET;
static const Socket BAD_SOCKET = INVALID_SOCKET;
static void closeSocket(Socket s){closesocket(s);}
static int socketError(){return WSAGetLastError();}
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using Socket = int;
static const Socket BAD_SOCKET = -1;
static void closeSocket(Socket s){close(s);}
static int socketError(){return errno;}
#endif

namespace {
std::mutex stateMutex;
std::atomic<bool> running{true};
HashTable siteTable(101);
CircularQueue alertQueue(100);
MaxHeap riskHeap;
MaintenanceLog maintenanceLog;
Graph drainageGraph(4);
std::vector<SiteRecord> configuredSites;
std::map<std::string, std::string> activeTokens;
std::string lastRefresh = "Not refreshed yet";
std::string lastApiError;
int nextMaintenanceId=1;
const int REFRESH_SECONDS=600;
const std::string DATA_FILE="../data/maintenance.csv";

std::string trim(const std::string& s){size_t a=s.find_first_not_of(" \t\r\n");if(a==std::string::npos)return "";size_t b=s.find_last_not_of(" \t\r\n");return s.substr(a,b-a+1);}
std::string lower(std::string s){for(char&c:s)c=(char)std::tolower((unsigned char)c);return s;}
std::string jsonEscape(const std::string& s){std::ostringstream o;for(unsigned char c:s){switch(c){case '"':o<<"\\\"";break;case '\\':o<<"\\\\";break;case '\n':o<<"\\n";break;case '\r':o<<"\\r";break;case '\t':o<<"\\t";break;default:if(c<32)o<<' ';else o<<c;}}return o.str();}
std::string jstr(const std::string& s){return "\""+jsonEscape(s)+"\"";}
std::string number(double n, int precision=1){std::ostringstream o;o.setf(std::ios::fixed);o.precision(precision);o<<n;return o.str();}
std::string bodyField(const std::string& body,const std::string& key){std::regex re("\\\""+key+"\\\"\\s*:\\s*\\\"((?:\\\\.|[^\\\"])*)\\\"");std::smatch m;if(!std::regex_search(body,m,re))return "";std::string v=m[1].str();v=std::regex_replace(v,std::regex("\\\\\""),"\"");v=std::regex_replace(v,std::regex("\\\\n")," ");return v;}
std::string now(){std::time_t t=std::time(nullptr);std::tm tm{};
#ifdef _WIN32
localtime_s(&tm,&t);
#else
localtime_r(&t,&tm);
#endif
std::ostringstream o;o<<std::put_time(&tm,"%Y-%m-%d %H:%M:%S");return o.str();}
std::string siteJson(const SiteRecord&s){std::ostringstream o;o<<"{\"id\":"<<jstr(s.id)<<",\"location\":"<<jstr(s.location)<<",\"latitude\":"<<number(s.latitude,4)<<",\"longitude\":"<<number(s.longitude,4)<<",\"precipitation\":"<<number(s.precipitation)<<",\"rain\":"<<number(s.rain)<<",\"humidity\":"<<number(s.humidity)<<",\"wind\":"<<number(s.wind)<<",\"risk\":"<<number(s.risk)<<",\"status\":"<<jstr(s.status)<<",\"observedAt\":"<<jstr(s.observedAt)<<",\"stale\":"<<(s.stale?"true":"false")<<",\"source\":\"Open-Meteo weather API\"}";return o.str();}
std::vector<SiteRecord> allSites(){auto v=siteTable.getAll();std::sort(v.begin(),v.end(),[](const auto&a,const auto&b){return a.id<b.id;});return v;}
std::string sitesJson(bool alertsOnly=false){std::ostringstream o;o<<"[";bool first=true;for(const auto&s:allSites()){if(alertsOnly&&s.risk<50)continue;if(!first)o<<",";first=false;o<<siteJson(s);}o<<"]";return o.str();}
std::string maintenanceJson(){auto v=maintenanceLog.newestFirst();std::ostringstream o;o<<"[";bool first=true;for(const auto&r:v){if(!first)o<<",";first=false;o<<"{\"id\":"<<jstr(r.id)<<",\"siteId\":"<<jstr(r.siteId)<<",\"location\":"<<jstr(r.location)<<",\"action\":"<<jstr(r.action)<<",\"note\":"<<jstr(r.note)<<",\"timestamp\":"<<jstr(r.timestamp)<<",\"by\":"<<jstr(r.by)<<"}";}o<<"]";return o.str();}
std::string graphJson(){auto order=drainageGraph.BFS(0);std::ostringstream o;o<<"{\"nodes\":[";for(int i=0;i<drainageGraph.getVertexCount();++i){if(i)o<<",";o<<"{\"id\":"<<i<<",\"name\":"<<jstr(drainageGraph.getLocation(i))<<"}";}o<<"],\"bfs\":[";for(size_t i=0;i<order.size();++i){if(i)o<<",";o<<jstr(drainageGraph.getLocation(order[i]));}o<<"]}";return o.str();}
void rebuildHeap(){riskHeap.clear();for(const auto&s:allSites())if(!s.observedAt.empty())riskHeap.insert({s.id,s.location,s.status,s.risk});}
void initialise(){
    configuredSites.clear();
    SiteRecord a; a.id="SD-101"; a.location="ISBT Drainage Zone"; a.latitude=30.2865; a.longitude=78.0127; configuredSites.push_back(a);
    SiteRecord b; b.id="SD-102"; b.location="Prince Chowk"; b.latitude=30.3247; b.longitude=78.0415; configuredSites.push_back(b);
    SiteRecord c; c.id="SD-103"; c.location="Sahastradhara Road"; c.latitude=30.3872; c.longitude=78.0865; configuredSites.push_back(c);
    SiteRecord d; d.id="SD-104"; d.location="Ballupur Chowk"; d.latitude=30.3348; d.longitude=77.9973; configuredSites.push_back(d);
    for(size_t i=0;i<configuredSites.size();++i)drainageGraph.setLocation((int)i,configuredSites[i].location);
    drainageGraph.addEdge(0,1);drainageGraph.addEdge(1,2);drainageGraph.addEdge(1,3);drainageGraph.addEdge(3,0);
    // Keep locations visible before the first API request, but clearly mark readings as unavailable.
    for(auto&s:configuredSites){s.status="Waiting for data";s.stale=true;siteTable.insertOrUpdate(s);}
    std::ifstream in(DATA_FILE);std::string line;while(std::getline(in,line)){std::vector<std::string>f;std::stringstream ss(line);std::string x;while(std::getline(ss,x,'|'))f.push_back(x);if(f.size()>=7){MaintenanceRecord r;r.id=f[0];r.siteId=f[1];r.location=f[2];r.action=f[3];r.note=f[4];r.timestamp=f[5];r.by=f[6];maintenanceLog.addRecord(r);try{nextMaintenanceId=std::max(nextMaintenanceId,std::stoi(r.id.substr(2))+1);}catch(...){}}}
}
void refreshWeather(){
    WeatherApiClient client; int successes=0; std::string firstError;
    for(const auto& base:configuredSites){
        SiteRecord next=base; std::string error; bool ok=client.fetchCurrent(next,error);
        std::lock_guard<std::mutex> lock(stateMutex);
        const SiteRecord* oldPtr=siteTable.find(base.id); SiteRecord old=oldPtr?*oldPtr:SiteRecord{};
        if(ok){
            siteTable.insertOrUpdate(next); ++successes;
            if(next.risk>=50 && old.status!=next.status) alertQueue.enqueue(next.id);
        } else {
            if(firstError.empty()) firstError=error;
            if(oldPtr){old.stale=true;siteTable.insertOrUpdate(old);}
            else{next.stale=true;next.status="Unavailable";siteTable.insertOrUpdate(next);}
        }
    }
    {std::lock_guard<std::mutex> lock(stateMutex);if(successes>0)lastRefresh=now();lastApiError=successes==(int)configuredSites.size()?"":firstError;rebuildHeap();}
    std::cout<<"[Weather] Successful updates: "<<successes<<"/"<<configuredSites.size()<<" at "<<now()<<"\n";
    if(!firstError.empty())std::cout<<"[Weather] "<<firstError<<"\n";
}
void refreshLoop(){while(running){refreshWeather();for(int i=0;i<REFRESH_SECONDS&&running;++i)std::this_thread::sleep_for(std::chrono::seconds(1));}}
std::string safeField(std::string s){for(char&c:s)if(c=='|'||c=='\n'||c=='\r')c=' ';return s;}
void saveMaintenance(const MaintenanceRecord&r){std::filesystem::create_directories("../data");std::ofstream out(DATA_FILE,std::ios::app);out<<safeField(r.id)<<"|"<<safeField(r.siteId)<<"|"<<safeField(r.location)<<"|"<<safeField(r.action)<<"|"<<safeField(r.note)<<"|"<<safeField(r.timestamp)<<"|"<<safeField(r.by)<<"\n";}
std::string makeToken(){static unsigned long long counter=0;return "sd-"+std::to_string((unsigned long long)std::time(nullptr))+"-"+std::to_string(++counter)+"-"+std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));}
bool authorized(const std::map<std::string,std::string>&headers){auto it=headers.find("x-auth-token");if(it==headers.end())return false;std::lock_guard<std::mutex> lock(stateMutex);return activeTokens.find(it->second)!=activeTokens.end();}
struct Request{std::string method,path,body;std::map<std::string,std::string>headers;};
struct Response{int status=200;std::string type="application/json; charset=utf-8";std::string body;};
Response jsonResponse(int status,const std::string&body){return {status,"application/json; charset=utf-8",body};}
std::string errorJson(const std::string&msg){return "{\"ok\":false,\"error\":"+jstr(msg)+"}";}
Response route(const Request&r){
    if(r.method=="OPTIONS")return {204,"text/plain",""};
    if(r.method=="GET"&&r.path=="/api/health"){std::lock_guard<std::mutex>l(stateMutex);return jsonResponse(200,"{\"ok\":true,\"service\":\"Storm Drain Telemetry\",\"source\":\"Open-Meteo\",\"lastRefresh\":"+jstr(lastRefresh)+",\"refreshSeconds\":"+std::to_string(REFRESH_SECONDS)+",\"lastError\":"+jstr(lastApiError)+"}");}
    if(r.method=="POST"&&r.path=="/api/admin/login"){
        std::string user=bodyField(r.body,"username"),pass=bodyField(r.body,"password");const char* eu=std::getenv("STORM_ADMIN_USER");const char* ep=std::getenv("STORM_ADMIN_PASSWORD");std::string expectedUser=eu?eu:"admin",expectedPass=ep?ep:"Drain@2026";
        if(user!=expectedUser||pass!=expectedPass) return jsonResponse(401,errorJson("Invalid username or password"));
        std::string token=makeToken();
        {std::lock_guard<std::mutex>l(stateMutex);activeTokens[token]=user;}
        return jsonResponse(200,"{\"ok\":true,\"token\":"+jstr(token)+",\"username\":"+jstr(user)+"}");
    }
    if(r.method=="GET"&&r.path=="/api/public/overview"){
        std::lock_guard<std::mutex>l(stateMutex);auto sites=allSites();int alerts=0,stale=0;for(const auto&s:sites){if(s.risk>=50)++alerts;if(s.stale)++stale;}RiskEntry top;std::string topJson="null";if(riskHeap.peekHighest(top))topJson="{\"id\":"+jstr(top.id)+",\"location\":"+jstr(top.location)+",\"risk\":"+number(top.score)+",\"status\":"+jstr(top.status)+"}";
        return jsonResponse(200,"{\"ok\":true,\"sites\":"+sitesJson()+",\"siteCount\":"+std::to_string(sites.size())+",\"alertCount\":"+std::to_string(alerts)+",\"staleCount\":"+std::to_string(stale)+",\"highestRisk\":"+topJson+",\"lastRefresh\":"+jstr(lastRefresh)+",\"lastError\":"+jstr(lastApiError)+",\"graph\":"+graphJson()+"}");
    }
    if(r.method=="GET"&&r.path=="/api/public/sites"){std::lock_guard<std::mutex>l(stateMutex);return jsonResponse(200,sitesJson());}
    if(r.method=="POST"&&r.path=="/api/admin/logout"){
        auto it=r.headers.find("x-auth-token");if(it==r.headers.end())return jsonResponse(401,errorJson("Admin login required"));
        std::lock_guard<std::mutex>l(stateMutex);activeTokens.erase(it->second);return jsonResponse(200,"{\"ok\":true}");
    }
    if(r.path.rfind("/api/admin/",0)==0&&!authorized(r.headers))return jsonResponse(401,errorJson("Admin login required"));
    if(r.method=="GET"&&r.path=="/api/admin/sites"){std::lock_guard<std::mutex>l(stateMutex);return jsonResponse(200,sitesJson());}
    if(r.method=="GET"&&r.path=="/api/admin/alerts"){std::lock_guard<std::mutex>l(stateMutex);return jsonResponse(200,"{\"alerts\":"+sitesJson(true)+",\"queue\":["+([&](){std::ostringstream o;auto q=alertQueue.getAll();for(size_t i=0;i<q.size();++i){if(i)o<<",";o<<jstr(q[i]);}return o.str();})()+"]}");}
    if(r.method=="GET"&&r.path=="/api/admin/maintenance"){std::lock_guard<std::mutex>l(stateMutex);return jsonResponse(200,maintenanceJson());}
    if(r.method=="GET"&&r.path=="/api/admin/graph"){std::lock_guard<std::mutex>l(stateMutex);return jsonResponse(200,graphJson());}
    if(r.method=="POST"&&r.path=="/api/admin/maintenance"){
        std::string siteId=bodyField(r.body,"siteId"),action=bodyField(r.body,"action"),note=bodyField(r.body,"note");if(siteId.empty()||action.empty())return jsonResponse(400,errorJson("Choose a location and maintenance action"));
        std::lock_guard<std::mutex>l(stateMutex);const SiteRecord*s=siteTable.find(siteId);if(!s)return jsonResponse(404,errorJson("Unknown drainage location"));if(action!="Inspection"&&action!="Cleaning"&&action!="Repair"&&action!="Follow-up")return jsonResponse(400,errorJson("Unsupported maintenance action"));
        auto it=r.headers.find("x-auth-token");std::string by="admin";if(it!=r.headers.end()&&activeTokens.count(it->second))by=activeTokens[it->second];MaintenanceRecord rec;rec.id="M-"+std::to_string(nextMaintenanceId++);rec.siteId=siteId;rec.location=s->location;rec.action=action;rec.note=note;rec.timestamp=now();rec.by=by;maintenanceLog.addRecord(rec);saveMaintenance(rec);return jsonResponse(201,"{\"ok\":true,\"message\":\"Maintenance record saved\",\"record\":{\"id\":"+jstr(rec.id)+",\"timestamp\":"+jstr(rec.timestamp)+"}}");
    }
    if(r.path.rfind("/api/",0)==0)return jsonResponse(404,errorJson("API endpoint not found"));
    return {404,"text/plain; charset=utf-8","Not found"};
}
std::string mimeType(const std::string&path){auto p=path.rfind('.');std::string ext=p==std::string::npos?"":lower(path.substr(p));if(ext==".html")return "text/html; charset=utf-8";if(ext==".css")return "text/css; charset=utf-8";if(ext==".js")return "application/javascript; charset=utf-8";if(ext==".svg")return "image/svg+xml";if(ext==".png")return "image/png";if(ext==".jpg"||ext==".jpeg")return "image/jpeg";if(ext==".ico")return "image/x-icon";return "application/octet-stream";}
std::string staticPath(const std::string&path){std::string p=path=="/"?"/index.html":path;if(p.find("..")!=std::string::npos)return "";return "../frontend"+p;}
void sendResponse(Socket s,const Response&res){std::string statusText=res.status==200?"OK":res.status==201?"Created":res.status==204?"No Content":res.status==400?"Bad Request":res.status==401?"Unauthorized":res.status==404?"Not Found":"Error";std::ostringstream h;h<<"HTTP/1.1 "<<res.status<<" "<<statusText<<"\r\nContent-Type: "<<res.type<<"\r\nContent-Length: "<<res.body.size()<<"\r\nConnection: close\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type, X-Auth-Token\r\nX-Content-Type-Options: nosniff\r\n\r\n";std::string all=h.str()+res.body;size_t sent=0;while(sent<all.size()){int n=(int)send(s,all.data()+sent,(int)(all.size()-sent),0);if(n<=0)break;sent+=(size_t)n;}}
void handleClient(Socket s){std::string raw;char buf[4096];size_t headerEnd=std::string::npos;size_t contentLength=0;while(raw.size()<1024*1024){int n=(int)recv(s,buf,sizeof(buf),0);if(n<=0)break;raw.append(buf,n);headerEnd=raw.find("\r\n\r\n");if(headerEnd!=std::string::npos){std::string hs=raw.substr(0,headerEnd);std::smatch m;if(std::regex_search(hs,m,std::regex("Content-Length:\\s*([0-9]+)", std::regex_constants::icase)))contentLength=(size_t)std::stoul(m[1].str());if(raw.size()>=headerEnd+4+contentLength)break;}}
    if(headerEnd==std::string::npos){sendResponse(s,{400,"text/plain","Malformed request"});closeSocket(s);return;}std::string head=raw.substr(0,headerEnd);std::istringstream hs(head);Request r;hs>>r.method>>r.path;std::string line;std::getline(hs,line);while(std::getline(hs,line)){if(line=="\r"||line.empty())continue;size_t c=line.find(':');if(c!=std::string::npos)r.headers[lower(trim(line.substr(0,c)))]=trim(line.substr(c+1));}r.body=raw.substr(headerEnd+4,contentLength);size_t q=r.path.find('?');if(q!=std::string::npos)r.path=r.path.substr(0,q);
    Response res;if(r.path.rfind("/api/",0)==0||r.method=="OPTIONS"){res=route(r);}else if(r.method=="GET"){std::string file=staticPath(r.path);std::ifstream in(file,std::ios::binary);if(file.empty()||!in){res={404,"text/plain; charset=utf-8","File not found"};}else{std::ostringstream content;content<<in.rdbuf();res={200,mimeType(file),content.str()};}}else res={405,"text/plain","Method not allowed"};sendResponse(s,res);closeSocket(s);
}
}

void runServer(){
    std::filesystem::create_directories("../data");initialise();
#ifdef _WIN32
    WSADATA wsa{};if(WSAStartup(MAKEWORD(2,2),&wsa)!=0){std::cerr<<"WSAStartup failed\n";return;}
#endif
    std::thread updater(refreshLoop);Socket listener=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);if(listener==BAD_SOCKET){std::cerr<<"Could not create server socket: "<<socketError()<<"\n";running=false;if(updater.joinable())updater.join();return;}
    int reuse=1;
#ifdef _WIN32
    setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,(const char*)&reuse,sizeof(reuse));
#else
    setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
#endif
    sockaddr_in addr{};addr.sin_family=AF_INET;addr.sin_addr.s_addr=htonl(INADDR_LOOPBACK);addr.sin_port=htons(8088);if(bind(listener,(sockaddr*)&addr,sizeof(addr))<0||listen(listener,16)<0){std::cerr<<"Could not bind port 8088. Close another server using this port.\n";closeSocket(listener);running=false;if(updater.joinable())updater.join();
#ifdef _WIN32
    WSACleanup();
#endif
    return;}
    std::cout<<"\nServer running at http://localhost:8088\nPublic dashboard: http://localhost:8088/\nAdmin dashboard:  http://localhost:8088/admin.html\nDemo admin login: admin / Drain@2026\nRefresh interval: every 10 minutes. Press Ctrl+C to stop.\n\n";
    while(running){sockaddr_in clientAddr{};
#ifdef _WIN32
        int len=sizeof(clientAddr);
#else
        socklen_t len=sizeof(clientAddr);
#endif
        Socket client=accept(listener,(sockaddr*)&clientAddr,&len);if(client==BAD_SOCKET){if(!running)break;continue;}std::thread(handleClient,client).detach();}
    closeSocket(listener);running=false;if(updater.joinable())updater.join();
#ifdef _WIN32
    WSACleanup();
#endif
}
