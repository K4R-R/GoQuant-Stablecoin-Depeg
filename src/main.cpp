#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <mutex>
#include <vector>
#include <set>
#include <sstream>
#include <conio.h>
#include <crow.h>
#include <thread>
#include <nlohmann/json.hpp>
#include "PriceMonitor.hpp"
#include "RiskScorer.hpp"
#include "AlertManager.hpp"
#include "ChainMonitor.hpp"
#include "HistoryManager.hpp"
#include "server.hpp"

using namespace std;

const vector<string> ALL_COINS = {"USDT", "USDC", "DAI", "FRAX", "BUSD"};

enum OutputOption {
    PRICE = 1,
    ONCHAIN = 2,
    HISTORY = 3,
    
};

bool show_output(int option, const set<int>& selected_outputs) {
    return selected_outputs.find(option) != selected_outputs.end();
}

int main() {
    thread server_thread(start_server);
    this_thread::sleep_for(chrono::milliseconds(200)); // Allow Crow logs to show first

    while (true) {
        // Coin selection
        cout << "\nAvailable coins to track:\n";
        for (int i = 0; i < ALL_COINS.size(); ++i)
            cout << i + 1 << ": " << ALL_COINS[i] << '\n';

        int coin_idx = -1;
        cout<< "Enter the number of the stablecoin to track: ";
        cin >> coin_idx;
        while (cin.fail() || coin_idx < 1 || coin_idx > ALL_COINS.size()) {
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "Invalid. Try again: ";
            cin >> coin_idx;
        }

        const string chosen_coin = ALL_COINS[coin_idx - 1];
        cout << "Tracking: " << chosen_coin << "\n" << endl;

        // Output module selection
        cout << "Select module outputs to display (comma-separated):\n";
        cout << "1: Real-time price monitoring\n";
        cout << "2: On-chain flow analysis\n";
        cout << "3: Historical pattern analysis\n> ";
        cin.ignore();
        string input;
        getline(cin, input);

        set<int> selected_outputs;
        stringstream ss(input);
        string temp;
        while (getline(ss, temp, ',')) {
            try {
                int val = stoi(temp);
                if (val >= 1 && val <= 3)
                    selected_outputs.insert(val);
            } catch (...) {}
        }

        PriceMonitor price_monitor;
        RiskScorer risk_scorer;
        AlertManager alert_manager;
        ChainMonitor chain_monitor;
        HistoryManager history_manager;

        // Tracking Loop
        while (true) {
            //Price monitoring
            auto prices = price_monitor.fetch_prices();

            if (show_output(PRICE, selected_outputs) && prices.count(chosen_coin)) {
                const auto& res = prices[chosen_coin];
                cout<<endl;
                cout << chosen_coin << " - Avg: $" << res.average_price;
                for (auto& [symbol, price] : res.source_prices) 
                    cout << " | " << symbol << ": $" << price;
                cout << endl;
            }

            // On-chain flow analysis
            vector<TransferEvent> transfers = chain_monitor.get_large_transfers(chosen_coin,50000.0);
            map<string, int> large_transfer_counts;
            large_transfer_counts[chosen_coin] = static_cast<int>(transfers.size());

            if (show_output(ONCHAIN, selected_outputs)) {
                cout << endl;
                cout << chosen_coin << " - Recent Large Transfers (> $50k): " << transfers.size() << endl;
                for(auto& evt : transfers) {
                    cout << "From: " << evt.from << ", To: " << evt.to << ", Value: $" << evt.value_usd << endl;
                }
            }

            double avg_price = prices[chosen_coin].average_price;
            RiskLevel risk = risk_scorer.calculate_risk(avg_price, transfers.size());

            // History management
            history_manager.load_from_file("history.json");
            time_t now = time(nullptr);
            char time_buf[20];
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

            HistoricalEntry entry;
            entry.timestamp = string(time_buf);
            entry.coin = chosen_coin;
            entry.avg_price = avg_price;
            entry.risk = risk;
            entry.large_transfer_count = static_cast<int>(transfers.size());

            history_manager.add_entry(entry);
            history_manager.save_to_file("history.json");

            double ma = history_manager.moving_average(chosen_coin);
            double sd = history_manager.moving_stdev(chosen_coin);
            bool price_alarm = history_manager.trend_alarm(chosen_coin);
            bool risk_alarm = history_manager.risk_trend_alarm(chosen_coin);

            if (show_output(HISTORY, selected_outputs)) {
                cout << endl << chosen_coin << " - 10 cycle avg: " << ma << " | stdev: " << sd << endl;
                if (price_alarm)
                    cout << "[TREND ALERT] " << chosen_coin << ": price below 0.999 for 5+ cycles." << endl;
                if (risk_alarm)
                    cout << "[RISK TREND ALERT] " << chosen_coin << ": HIGH risk for 3+ cycles." << endl;
            }

            // Display Alert
            alert_manager.process_alert(chosen_coin , avg_price , risk);


            // Menu input options
            cout<<endl;
            cout << "--- Waiting 2 seconds before next analysis ---" << endl;
            cout << "1 -> Go back to main menu\n2 -> Exit\n(Press nothing to continue)" << endl;

            this_thread::sleep_for(chrono::seconds(2));

            if (_kbhit()) {
                char c = _getch();
                if (c == '1') break;     // Go back to main menu
                if (c == '2') exit(0);   // Exit program
            }
        }
    }

    return 0;
}
