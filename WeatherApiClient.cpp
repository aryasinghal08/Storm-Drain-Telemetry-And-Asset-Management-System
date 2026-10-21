#include "WeatherApiClient.h"
#include <regex>
#include <sstream>
#include <iomanip>
#include <ctime>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <winhttp.h>
#endif

namespace {
std::string jsonValue(const std::string& object, const std::string& key) {
    std::regex re("\\\"" + key + "\\\"\\s*:\\s*(\\\"[^\\\"]*\\\"|-?[0-9]+(?:\\.[0-9]+)?|null)");
    std::smatch m; if (!std::regex_search(object, m, re)) return "";
    std::string v=m[1].str(); if(v.size()>=2 && v.front()=='"')v=v.substr(1,v.size()-2); return v=="null"?"":v;
}
double toDouble(const std::string& s) { try { return s.empty()?0.0:std::stod(s); } catch (...) { return 0.0; } }
std::string localTimestamp() { std::time_t t=std::time(nullptr); std::tm tm{};
#ifdef _WIN32
 localtime_s(&tm,&t);
#else
 localtime_r(&t,&tm);
#endif
 std::ostringstream o;o<<std::put_time(&tm,"%Y-%m-%d %H:%M:%S");return o.str(); }
#ifdef _WIN32
bool httpsGet(const std::wstring& host,const std::wstring& path,std::string& body,std::string& error) {
    HINTERNET session=WinHttpOpen(L"StormDrainTelemetry/1.0",WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0);
    if(!session){error="Could not initialize HTTPS client";return false;}
    WinHttpSetTimeouts(session,5000,5000,7000,7000);
    HINTERNET connection=WinHttpConnect(session,host.c_str(),INTERNET_DEFAULT_HTTPS_PORT,0);
    if(!connection){WinHttpCloseHandle(session);error="Could not connect to weather API";return false;}
    HINTERNET request=WinHttpOpenRequest(connection,L"GET",path.c_str(),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_SECURE);
    if(!request){WinHttpCloseHandle(connection);WinHttpCloseHandle(session);error="Could not create API request";return false;}
    bool ok=false;
    if(WinHttpSendRequest(request,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,0) && WinHttpReceiveResponse(request,nullptr)){
        DWORD status=0, statusSize=sizeof(status);
        WinHttpQueryHeaders(request,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,WINHTTP_HEADER_NAME_BY_INDEX,&status,&statusSize,WINHTTP_NO_HEADER_INDEX);
        if(status>=200&&status<300){DWORD available=0;while(WinHttpQueryDataAvailable(request,&available)&&available>0){std::string chunk(available,'\0');DWORD read=0;if(!WinHttpReadData(request,chunk.data(),available,&read))break;chunk.resize(read);body+=chunk;if(read==0)break;}ok=true;}
        else error="Weather API returned HTTP status "+std::to_string(status);
    } else error="Weather API request failed; check internet connectivity";
    WinHttpCloseHandle(request);WinHttpCloseHandle(connection);WinHttpCloseHandle(session);return ok;
}
#endif
}

bool WeatherApiClient::fetchCurrent(SiteRecord& site, std::string& error) const {
#ifdef _WIN32
    std::wostringstream path;path<<L"/v1/forecast?latitude="<<std::fixed<<std::setprecision(4)<<site.latitude<<L"&longitude="<<site.longitude<<L"&current=precipitation,rain,relative_humidity_2m,wind_speed_10m&timezone=Asia%2FKolkata";
    std::string body;
    if(!httpsGet(L"api.open-meteo.com",path.str(),body,error))return false;
    std::smatch match;std::regex currentRe("\\\"current\\\"\\s*:\\s*\\{([^}]*)\\}");
    if(!std::regex_search(body,match,currentRe)){error="Weather API response did not contain current observations";return false;}
    const std::string current=match[1].str();
    std::string precip=jsonValue(current,"precipitation"), rain=jsonValue(current,"rain"), humidity=jsonValue(current,"relative_humidity_2m"), wind=jsonValue(current,"wind_speed_10m"), observed=jsonValue(current,"time");
    if(precip.empty()||humidity.empty()){error="Weather API response is missing required fields";return false;}
    site.precipitation=toDouble(precip);site.rain=toDouble(rain);site.humidity=toDouble(humidity);site.wind=toDouble(wind);
    site.observedAt=observed.empty()?localTimestamp():observed;site.stale=false;
    // This is a transparent prototype indicator, not a validated flood forecast.
    site.risk=site.rain*12.0+site.precipitation*8.0+site.humidity*0.12+site.wind*0.15;
    if(site.risk>100)site.risk=100;if(site.risk<0)site.risk=0;
    if(site.risk>=75)site.status="High";else if(site.risk>=50)site.status="Elevated";else if(site.risk>=30)site.status="Watch";else site.status="Normal";
    return true;
#else
    (void)site;error="Automatic weather API requests are implemented for Windows using WinHTTP. Run this project on Windows/MSYS2 UCRT64.";return false;
#endif
}
