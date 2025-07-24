#include "PriceMonitor.hpp"               // Declaration of PriceMonitor and PriceResult
#include <curl/curl.h>                    // For making HTTP requests
#include <nlohmann/json.hpp>              // For JSON parsing
#include <iostream>
#include <string>
#include <map>

using namespace std;
using json = nlohmann::json;

// Your API key for CoinGecko
const string COINGECKO_API_KEY = "CG-WL7w8K7LUVAN2xJA1wAFjRr9";
const string COINCAP_API_KEY = "d7af78f04eae2eb2d4aac04178a30229f2f307ec54db5bd88e206101d4c5dd89";

// List of supported coin IDs and their symbols
map<string, string> coin_id_map_gecko = {
    {"tether", "USDT"},
    {"usd-coin", "USDC"},
    {"dai", "DAI"},
    {"frax", "FRAX"},
    {"binance-usd", "BUSD"}
};
map<string, string> coin_id_map_cap = {
    {"tether", "USDT"},
    {"usd-coin", "USDC"},
    {"multi-collateral-dai", "DAI"},
    {"frax", "FRAX"},
    {"binance-usd", "BUSD"}
};

// Callback function to append received curl data to a string
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Helper function to perform HTTP GET with header
static bool fetch_url(const string& url, string& response, const vector<string>& headers) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* header_list = nullptr;
    for (const auto& h : headers)
        header_list = curl_slist_append(header_list, h.c_str());

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK);
}

map<string, PriceResult> PriceMonitor::fetch_prices() {
    map<string, PriceResult> result;

    // -------- CoinGecko (Authenticated) --------
    string gecko_url = "https://api.coingecko.com/api/v3/simple/price?ids=";
    for (auto it = coin_id_map_gecko.begin(); it != coin_id_map_gecko.end(); ++it) {
        if (it != coin_id_map_gecko.begin()) gecko_url += ",";
        gecko_url += it->first;
    }
    gecko_url += "&vs_currencies=inr,usd";

    string cg_response;
    vector<string> cg_headers = {"x-cg-demo-api-key: " + COINGECKO_API_KEY};

    if (fetch_url(gecko_url, cg_response, cg_headers)) {
        json data = json::parse(cg_response, nullptr, false);
        if (!data.is_discarded()) {
            for (auto& [cg_id, symbol] : coin_id_map_gecko) {
                if (data.contains(cg_id) && data[cg_id].contains("usd")) {
                    result[symbol].source_prices["CoinGecko"] = data[cg_id]["usd"];
                }
            }
        }
    } else {
        cout << "[ERROR] CoinGecko fetch failed.\n";
    }

    // -------- CoinCap (Authenticated) --------
    string cap_url = "https://rest.coincap.io/v3/assets?ids=";
    for (auto it = coin_id_map_cap.begin(); it != coin_id_map_cap.end(); ++it) {
        if (it != coin_id_map_cap.begin()) cap_url += ",";
        cap_url += it->first;
    }

    string cap_response;
    vector<string> cap_headers = {
        "Authorization: Bearer " + COINCAP_API_KEY,
        "Accept: application/json"
    };

    if (fetch_url(cap_url, cap_response, cap_headers)) {
        // cout << "Raw CoinCap response:\n" << cap_response << "\n";
        json data = json::parse(cap_response, nullptr, false);
        if (data.contains("data")) {
            for (auto& asset : data["data"]) {
                string id = asset["id"];
                if (coin_id_map_cap.count(id) && asset.contains("priceUsd")) {
                    string symbol = coin_id_map_cap[id];
                    result[symbol].source_prices["CoinCap"] = stod(string(asset["priceUsd"]));
                }
            }
        }
    } else {
        cout << "[ERROR] CoinCap fetch failed.\n";
    }

    for (auto& [symbol, priceResult] : result) {
        double sum = 0.0;
        int count = 0;

        for (const auto& [source, price] : priceResult.source_prices) {
            sum += price;
            count++;
        }

        if (count > 0)
            priceResult.average_price = sum / count;
        else
            priceResult.average_price = 0.0;
    }

    return result;
}

