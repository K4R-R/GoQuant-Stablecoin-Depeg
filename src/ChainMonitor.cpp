#include "ChainMonitor.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <iostream>
#include <cmath>

using namespace std;
using json = nlohmann::json;

// Etherscan API key
const string ETHERSCAN_API_KEY = "B4EH87HTJJR9G1RB7KJXU9B9TDBRP2EPFR";

// Token contract addresses
unordered_map<string, string> token_contracts = {
    {"USDT", "0xdAC17F958D2ee523a2206206994597C13D831ec7"},
    {"USDC", "0xA0b86991C6218b36c1d19D4a2e9Eb0cE3606EB48"},
    {"DAI",  "0x6B175474E89094C44Da98b954EedeAC495271d0F"},
    {"FRAX", "0x853d955aCEf822Db058eb8505911ED77F175b99e"},
    {"BUSD", "0x4fabb145d64652a948d72533023f6e7a623c7c53"}
};

// Decimal precision for each token
unordered_map<string, int> token_decimals = {
    {"USDT", 6}, {"USDC", 6}, {"DAI", 18}, {"FRAX", 18}, {"BUSD", 18}
};

// Writes the API response into a string buffer
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Fetches 50 most recent transfers
vector<TransferEvent> ChainMonitor::get_large_transfers(const string& symbol, double min_usd) {
    vector<TransferEvent> result;

    if (!token_contracts.count(symbol)) return result;

    string url = "https://api.etherscan.io/api?module=account&action=tokentx"
                 "&contractaddress=" + token_contracts[symbol] +
                 "&page=1&offset=50&sort=desc&apikey=" + ETHERSCAN_API_KEY;

    string response;
    CURL* curl = curl_easy_init();
    if (!curl) return result;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    auto data = json::parse(response, nullptr, false);
    if (!data.contains("result")) return result;

    for (const auto& tx : data["result"]) {
        int decimals = stoi(string(tx["tokenDecimal"]));
        double raw = stod(string(tx["value"]));
        double amount = raw / pow(10.0, decimals);

        if (amount >= min_usd) {
            TransferEvent evt;
            evt.from = tx["from"];
            evt.to = tx["to"];
            evt.value_usd = amount;
            result.push_back(evt);
        }
    }

    return result;
}
