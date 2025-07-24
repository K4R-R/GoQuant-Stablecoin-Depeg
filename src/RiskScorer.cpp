#include "RiskScorer.hpp"
#include <cmath>

using namespace std;

RiskLevel RiskScorer::calculate_risk(double price, int large_transfer_count) {
    double deviation = fabs(price - 1.0);
    RiskLevel level;

    if (deviation <= 0.0001)
        level = LOW;
    else if (deviation <= 0.001)
        level = MEDIUM;
    else
        level = HIGH;

    if (large_transfer_count >= 3 && level != HIGH) {
        level = (level == LOW) ? MEDIUM : HIGH;
    }

    return level;
}
