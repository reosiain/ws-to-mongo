#include "websocket.h"

int main() {

    std::string url = "wss://wspap.okx.com:8443/ws/v5/public?brokerId=9999";
    std::string params = "{\n"
                         "  \"op\": \"subscribe\",\n"
                         "  \"args\": [\n"
                         "    {\n"
                         "      \"channel\": \"candle1m\",\n"
                         "      \"instId\": \"DOT-USDT\"\n"
                         "    }]}";

    try {

        Ws socket = Ws(url, params);
        socket.connect();

    } catch (const std::exception & e) {
        std::cout << e.what() << std::endl;
    } catch (websocketpp::lib::error_code &e) {
        std::cout << e.message() << std::endl;
    } catch (...) {
        std::cout << "Some " << std::endl;
    };

    return 0;
}