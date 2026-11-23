
#define WEATHERAPICLIENT_H
#include "HashTable.h"
#include <string>

class WeatherApiClient {
public:
    bool fetchCurrent(SiteRecord& site, std::string& error) const;
};
#endif
