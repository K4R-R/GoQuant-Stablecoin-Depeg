#include <crow.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>
#include "PriceMonitor.hpp"
#include "ChainMonitor.hpp"
#include "HistoryManager.hpp"

using json = nlohmann::json;
using namespace std;

string to_upper(string s) {
    transform(s.begin(), s.end(), s.begin(),
                [](unsigned char c){ return toupper(c); });
    return s;
}

void start_server() {
    crow::SimpleApp app;
    PriceMonitor price_monitor;
    ChainMonitor chain_monitor;
    HistoryManager history_manager;

    CROW_ROUTE(app, "/")([]() {
        return "Server is running";
    });

    CROW_ROUTE(app, "/price")([&price_monitor]() {
        auto prices = price_monitor.fetch_prices();
        json response;

        for (const auto& [coin, res] : prices) {
            json coin_data;
            coin_data["average_price"] = res.average_price;

            json sources;
            for (const auto& [exchange, price] : res.source_prices) {
                sources[exchange] = price;
            }

            coin_data["sources"] = sources;
            response[coin] = coin_data;
        }

        return crow::response{response.dump(4)};
    });

    // New /price/<coin> endpoint for specific coin
    CROW_ROUTE(app, "/price/<string>")([&price_monitor](const string& coin_param) {
        string coin = to_upper(coin_param);
        auto prices = price_monitor.fetch_prices();
        json response;

        auto it = prices.find(coin);
        if (it == prices.end()) {
            // Coin not found: return 404 or message
            return crow::response(404, "Coin not found");
        }

        const auto& res = prices[coin];
        json coin_data;
        coin_data["average_price"] = res.average_price;

        json sources;
        for (const auto& [exchange, price] : res.source_prices) {
            sources[exchange] = price;
        }

        coin_data["sources"] = sources;
        response[coin] = coin_data;

        return crow::response{response.dump(4)};
    });

    CROW_ROUTE(app, "/onchain/<string>")([&chain_monitor](const string& coin_param) {
        string coin = to_upper(coin_param);
        double min_usd = 50000.0;

        auto transfers = chain_monitor.get_large_transfers(coin, min_usd);

        if (transfers.empty()) {
            return crow::response(404, "No large transfers found or invalid coin symbol");
        }

        json result_json = json::array();
        for (const auto& evt : transfers) {
            result_json.push_back({
                {"from", evt.from},
                {"to", evt.to},
                {"value_usd", evt.value_usd}
            });
        }
        return crow::response{result_json.dump(4)};
    });

    CROW_ROUTE(app, "/history/<string>")([&history_manager](const string& coin_param) {
        string coin = to_upper(coin_param);

        history_manager.load_from_file("history.json");
        double avg = history_manager.moving_average(coin);
        double stdev = history_manager.moving_stdev(coin);
        bool trend = history_manager.trend_alarm(coin);
        bool risk = history_manager.risk_trend_alarm(coin);

        if (avg == 0.0 && stdev == 0.0 && !trend && !risk) {
            return crow::response(404, "No history found for coin: " + coin);
        }

        json res;
        res["coin"] = coin;
        res["moving_average"] = avg;
        res["moving_stdev"] = stdev;
        res["trend_alarm"] = trend;
        res["risk_trend_alarm"] = risk;

        return crow::response{res.dump(4)};
    });


    app.port(8080).multithreaded().run();
}