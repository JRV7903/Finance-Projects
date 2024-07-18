#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

const string API_KEY = "your_api_key";
const string SECRET_KEY = "your_secret_key";
const string BASE_URL = "https://paper-api.alpaca.markets";

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string make_request(const string& url, const string& method = "GET", const string& payload = "") {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("APCA-API-KEY-ID: " + API_KEY).c_str());
        headers = curl_slist_append(headers, ("APCA-API-SECRET-KEY: " + SECRET_KEY).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        } else if (method == "DELETE") {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        }

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    return readBuffer;
}

json get_current_price(const string& symbol) {
    string url = BASE_URL + "/v2/stocks/" + symbol + "/quote";
    string response = make_request(url);
    return json::parse(response);
}

void place_order(const string& symbol, int qty, const string& side, const string& type, const string& time_in_force) {
    string url = BASE_URL + "/v2/orders";
    json payload = {
        {"symbol", symbol},
        {"qty", qty},
        {"side", side},
        {"type", type},
        {"time_in_force", time_in_force}
    };
    make_request(url, "POST", payload.dump());
}

int main() {
    string symbol = "AAPL";
    int qty = 1;

    json current_price_data = get_current_price(symbol);
    double current_price = current_price_data["last"]["price"];

    // Simple momentum strategy: buy if the price is up, sell if the price is down
    static double previous_price = 0;

    if (previous_price > 0) {
        if (current_price > previous_price) {
            place_order(symbol, qty, "buy", "market", "gtc");
            cout << "Buying " << qty << " shares of " << symbol << " at $" << current_price << endl;
        } else if (current_price < previous_price) {
            place_order(symbol, qty, "sell", "market", "gtc");
            cout << "Selling " << qty << " shares of " << symbol << " at $" << current_price << endl;
        }
    }

    previous_price = current_price;

    return 0;
}
