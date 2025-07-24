#pragma once
#include <string>
#include <map>
#include <vector>

struct PriceResult {
    std::map<std::string, double> source_prices; // Source name -> price
    double average_price;
};

class PriceMonitor {
public:
    // Fetches prices for all coins from all sources
    std::map<std::string, PriceResult> fetch_prices();
};
