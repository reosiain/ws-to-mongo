#include <boost/thread.hpp>
#include "logger.h"
#include "websocket.h"
#include "msg_handler.h"
#include <vector>
#include <fstream>
#include <boost/exception/diagnostic_information.hpp>
#include <stdlib.h>


void run_ws(std::string url, std::string params, std::vector<json>* input_container, boost::barrier& bar){

    try {

        Ws socket = Ws(url, params, input_container);
        bar.wait();
        socket.connect();

    } catch (const std::exception & e) {
        BOOST_LOG_SEV(lg, ERROR) << e.what();
    } catch (websocketpp::lib::error_code &e) {
        BOOST_LOG_SEV(lg, ERROR) << e.message();
    } catch (...) {
        BOOST_LOG_SEV(lg, ERROR) << "Unexpected error";
    };

};

void run_pusher(std::string db_url, std::vector<json>* input_container, std::vector<json>* output_container, boost::barrier& bar){

    try {
        MongoPusher C = MongoPusher(db_url);
        bar.wait();
        C.monitor(input_container, output_container);

    } catch(const std::exception& e) {
        BOOST_LOG_SEV(lg, ERROR) << e.what();
        std::string st = boost::diagnostic_information(e);
        BOOST_LOG_SEV(lg, ERROR) << st;
    };

};

mongocxx::instance MongoPusher::inst = mongocxx::instance{};

int main() {
        
    try{       
        std::string url = std::getenv("WS_ADDRESS");
        std::string db_url = std::getenv("MONGO_URI");
        std::string params_path = std::getenv("SUBSCRIPTION_PARAMS");
        std::string logger_path = std::getenv("WS_LOGGER_PATH");

        std::cout << "Starting..\n";
        init_logging(logger_path);
        BOOST_LOG_SEV(lg, DEBUG) << db_url;

        std::ifstream ifs(params_path.c_str());
        json j = json::parse(ifs);
        std::string params = j.dump();

        std::vector<json> input_container;
        std::vector<json> output_container;

        boost::barrier bar(2);
        boost::thread_group threads;
        threads.create_thread(boost::bind(&run_ws, url, params, &input_container, boost::ref(bar)));
        threads.create_thread(boost::bind(&run_pusher, db_url, &input_container, &output_container, boost::ref(bar)));

        threads.join_all();

    }catch (const std::exception& e) {
        std::string st = boost::diagnostic_information(e);
        BOOST_LOG_SEV(lg, FATAL) << st;
    }

    return 0;
}
