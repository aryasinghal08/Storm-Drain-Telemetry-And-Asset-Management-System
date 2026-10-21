#ifndef WEATHERAPICLIENT_H
#define WEATHERAPICLIENT_H
#include "HashTable.h"
#include <string>

class WeatherApiClient {
public:
    // Fetches live weather observations. These are not physical drain-sensor readings.
    bool fetchCurrent(SiteRecord& site, std::string& error) const;
};
#endif
