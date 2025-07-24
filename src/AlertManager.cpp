#include "AlertManager.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <ctime>

using namespace std;
using json = nlohmann::json;

string risk_to_str(RiskLevel level) {
    switch (level) {
        case LOW: return "LOW";
        case MEDIUM: return "MEDIUM";
        case HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}

// Simple timestamp function
string get_current_timestamp() {
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

// Process one alert with timestamp
void AlertManager::process_alert(const string& symbol, double price, RiskLevel risk) {
    if (risk == MEDIUM || risk == HIGH) {
        cout << "[ALERT] " << symbol << " risk is " << risk_to_str(risk)
             << " at price $" << price << endl;

        json alert = {
            {"timestamp", get_current_timestamp()},
            {"coin", symbol},
            {"price", price},
            {"risk", risk_to_str(risk)}
        };

        // Read existing alerts into a JSON array
        json alert_log = json::array();
        ifstream in("alerts.json");
        if (in.is_open()) {
            try {
                in >> alert_log;
            } catch (...) {
                alert_log = json::array(); // Reset if file is corrupt
            }
            in.close();
        }

        // Append new alert
        alert_log.push_back(alert);

        // Keep only the last 100 entries
        if (alert_log.size() > 100)
            alert_log.erase(alert_log.begin(), alert_log.end() - 100);

        // Overwrite the file with updated array
        ofstream out("alerts.json");
        out << alert_log.dump(4) << endl;
    }
}
