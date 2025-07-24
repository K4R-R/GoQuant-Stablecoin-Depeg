#pragma once
#include <string>

enum RiskLevel {
    LOW,
    MEDIUM,
    HIGH
};

class RiskScorer {
public:
    RiskLevel calculate_risk(double price, int large_transfer_count);
};

std::string risk_to_str(RiskLevel level);
