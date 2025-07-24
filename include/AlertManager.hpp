#pragma once
#include <string>
#include "RiskScorer.hpp"

class AlertManager {
public:
    void process_alert(const std::string& symbol, double price, RiskLevel risk);
};
