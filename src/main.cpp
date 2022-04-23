#include "websocket.h"
#include "msg_handler.h"
#include <vector>
#include <thread>

void run_ws(std::string &url, std::string &params, std::vector<json> &input_container){

    try {

        Ws socket = Ws(url, params, input_container);
        socket.connect();

    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    } catch (websocketpp::lib::error_code &e) {
        std::cout << e.message() << std::endl;
    } catch (...) {
        std::cout << "Unexpected error" << std::endl;
    };

};

void run_pusher(std::string &db_url, std::vector<json> &input_container, std::vector<json> &output_container){

    try {
        MongoPusher C = MongoPusher(db_url);
        C.monitor(input_container, output_container);
    } catch (...) {
        std::cout << "Unexpected error in pusher" << std::endl;
    };

};


int main() {

    std::string url = "wss://wspap.okx.com:8443/ws/v5/public?brokerId=9999";
    std::string db_url = "mongodb://localhost:27017";
    std::string params = "{\n"
                         "  \"op\": \"subscribe\",\n"
                         "  \"args\": [\n"
                         "    {\n"
                         "      \"channel\": \"candle1m\",\n"
                         "      \"instId\": \"DOT-USDT\"\n"
                         "    }]}";

    std::vector<json> input_container(10);
    std::vector<json> output_container(10);

    std::thread run_ws(url, params, input_container);
    std::thread run_pusher(db_url, input_container, output_container);


    return 0;
}