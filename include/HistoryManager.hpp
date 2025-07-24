#pragma once
#include <string>
#include <vector>
#include "RiskScorer.hpp"

struct HistoricalEntry {
    std::string timestamp;
    std::string coin;
    double avg_price;
    RiskLevel risk;
    int large_transfer_count;
};

class HistoryManager {
public:
    void add_entry(const HistoricalEntry& entry);
    void save_to_file(const std::string& filename) const;
    void load_from_file(const std::string& filename);

    double moving_average(const std::string& coin, size_t window = 10) const;
    double moving_stdev(const std::string& coin, size_t window = 10) const;
    bool trend_alarm(const std::string& coin, size_t window = 5, double price_thresh = 0.999) const;
    bool risk_trend_alarm(const std::string& coin, size_t window = 3) const;

private:
    std::vector<HistoricalEntry> history;
    const size_t max_history = 100;
};
