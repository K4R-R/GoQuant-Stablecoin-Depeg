#include "HistoryManager.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;
using json = nlohmann::json;

void HistoryManager::add_entry(const HistoricalEntry& entry) {
    if (history.size() >= max_history)
        history.erase(history.begin());
    history.push_back(entry);
}

void HistoryManager::load_from_file(const string& filename) {
    history.clear();
    std::ifstream in(filename);
    if (!in.is_open()) return;

    nlohmann::json data;
    in >> data;
    for (const auto& item : data) {
        HistoricalEntry e;
        e.timestamp = item["timestamp"];
        e.coin = item["coin"];
        e.avg_price = item["avg_price"];
        string risk_str = item["risk"];
        if (risk_str == "LOW") e.risk = LOW;
        else if (risk_str == "MEDIUM") e.risk = MEDIUM;
        else e.risk = HIGH;
        e.large_transfer_count = item["large_transfer_count"];
        history.push_back(e);
    }
}


void HistoryManager::save_to_file(const string& filename) const {
    if (history.empty()) return;
    cout<<history.size()<<endl;
    const auto& e = history.back();

    json entry = {
        {"timestamp", e.timestamp},
        {"coin", e.coin},
        {"avg_price", e.avg_price},
        {"risk", risk_to_str(e.risk)},
        {"large_transfer_count", e.large_transfer_count}
    };

    json history_log = json::array();
    ifstream in(filename);
    if (in.is_open()) {
        try {
            in >> history_log;
        } catch (...) {
            history_log = json::array();
        }
        in.close();
    }

    history_log.push_back(entry);

    // Keep only the last 100 entries
    if (history_log.size() > 100)
        history_log.erase(history_log.begin(), history_log.end() - 100);

    ofstream out(filename);
    out << history_log.dump(4) << endl;
}

double HistoryManager::moving_average(const string& coin, size_t window) const {
    if (history.empty()) {
        cout<<"here"<<endl;
        return 0.0;
    }
    double sum = 0;
    int count = 0;

    for (int i = history.size() - 1; i >= 0 && count < window; --i) {
        if (history[i].coin == coin) {
            sum += history[i].avg_price;
            count++;
        }
    }

    if(count) return sum/count;
    else return 0.0; // No data for this coin
}

double HistoryManager::moving_stdev(const string& coin, size_t window) const {
    if (history.empty()) return 0.0;
    double stdev = 0.0;
    double avg = moving_average(coin,window);
    double variance = 0.0;
    int count = 0;

    for (int i = history.size() - 1; i >= 0 && count < window; --i) {
        const auto& e = history[i];
        if (e.coin == coin) {
            double diff = e.avg_price - avg;
            variance += diff * diff;
            count++;
        }
    }

    if (count > 0) {
        stdev = sqrt(variance / count);
    }

    return stdev;
}

bool HistoryManager::trend_alarm(const string& coin, size_t window, double thresh) const {
    if (history.empty()) return false;
    int count = 0;
    bool belowThresh = false;

    for (int i = history.size() - 1; i >= 0 && count < window; --i) {
        const auto& e = history[i];
        if (e.coin == coin) {
            if (e.avg_price < thresh) {
                belowThresh = true;
            } else {
                belowThresh = false;
            }
            count++;
        }
    }

    return belowThresh;
}

bool HistoryManager::risk_trend_alarm(const string& coin, size_t window) const {
    if (history.empty()) return false;
    int count = 0;
    bool highRisk = false;

    for (int i = history.size() - 1; i >= 0 && count < window; --i) {
        const auto& e = history[i];
        if (e.coin == coin) {
            if (e.risk == HIGH) {
                highRisk = true;
            } else {
                highRisk = false;
            }
            count++;
        }
    }

    return highRisk;
}
