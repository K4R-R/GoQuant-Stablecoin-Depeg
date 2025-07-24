#pragma once
#include <string>
#include <vector>

struct TransferEvent {
    std::string from;
    std::string to;
    double value_usd; // Approximate value in USD
};

class ChainMonitor {
public:
    std::vector<TransferEvent> get_large_transfers(const std::string& symbol, double min_usd = 50000.0);
};
