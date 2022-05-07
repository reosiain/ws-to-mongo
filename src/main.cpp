#include "websocket.h"
#include "msg_handler.h"
#include <vector>
#include <thread>
#include <fstream>
#include <boost/exception/diagnostic_information.hpp>

void run_ws(std::string url, std::string params, std::vector<json>* input_container){

    try {

        Ws socket = Ws(url, params, input_container);
        socket.connect();

    } catch (const std::exception & e) {
        std::cerr << e.what() << std::endl;
    } catch (websocketpp::lib::error_code &e) {
        std::cout << e.message() << std::endl;
    } catch (...) {
        std::cout << "Unexpected error" << std::endl;
    };

};

void run_pusher(std::string db_url, std::vector<json>* input_container, std::vector<json>* output_container){

    try {
        MongoPusher C = MongoPusher(db_url);
        C.monitor(input_container, output_container);

    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        std::string st = boost::diagnostic_information(e);
        std::cerr << st << '\n';
    };

};

mongocxx::instance MongoPusher::inst = mongocxx::instance{};

int main() {
        
    try{       
        std::string url = std::getenv("WS_ADDRESS");
        std::string db_url = std::getenv("MONGO_URI");
        std::string params_path = std::getenv("SUBSCRIPTION_PARAMS");

        std::ifstream ifs(params_path.c_str());
        json j = json::parse(ifs);
        std::string params = j.dump();

        std::vector<json> input_container;
        std::vector<json> output_container;

        std::vector<std::thread> threads;
        std::thread thread_ws(run_ws, url, params, &input_container);
        std::thread thread_pu(run_pusher, db_url, &input_container, &output_container);

        threads.push_back(std::move(thread_ws));
        threads.push_back(std::move(thread_pu));

        for (auto& t : threads) {
            t.join();
            std::cout << "Thread started" << std::endl;
        }
    }catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        std::string st = boost::diagnostic_information(e);
        std::cerr << st << '\n';
    }

    return 0;
}
