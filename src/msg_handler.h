#pragma once
#ifndef WS_TO_MONGO_MSG_HANDLER_H
#define WS_TO_MONGO_MSG_HANDLER_H


#include <mutex>
#include <cstdint>
#include <iostream>
#include <vector>
#include <chrono>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include "websocket.h"
#include <boost/thread/thread.hpp>
#include <boost/exception/diagnostic_information.hpp>

using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using json = nlohmann::json;

class MongoPusher{
public:
    explicit MongoPusher (std::string &db_url){

        uri = mongocxx::uri(db_url);
        client = mongocxx::client(uri);
        client.start_session();
        std::string db_name = std::getenv("DB_NAME");
        std::string db_coll = std::getenv("OHLC_COLLECTION_NAME");
        collection = client[db_name][db_coll];

    }

    void push_to_db(std::vector<json>* output_container){

        std::vector<bsoncxx::document::value> docs;

        for(auto&& elem : *output_container) {
            auto instId= elem["arg"]["instId"].get<std::string>();
            auto data = elem["data"].get<std::vector<std::vector<std::string>>>();
            bsoncxx::document::value document = make_document(
                    kvp("instId", instId),
                    kvp("dt", std::stof(data[0][0])),
                    kvp("o", std::stof(data[0][1])),
                    kvp("h", std::stof(data[0][2])),
                    kvp("l", std::stof(data[0][3])),
                    kvp("c", std::stof(data[0][4])),
                    kvp("vol_count", std::stof(data[0][5])),
                    kvp("vol_coin", std::stof(data[0][6]))
            );

            docs.push_back(document);
        };
        std::vector<json> _c;
        *output_container = _c;
        collection.insert_many(docs);

    };

    void monitor(std::vector<json>* input_container, std::vector<json>* output_container){

        try{
            while(true) {

                if (input_container->size() >= 10) {
                    std::unique_lock<std::mutex> lck(m);
                    lck.unlock();
                    *output_container = *input_container;
                    std::vector<json> _c;
                    *input_container = _c;
                    push_to_db(output_container);
                    lck.lock();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                };
            };

        }catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
            std::string st = boost::diagnostic_information(e);
            std::cerr << st << '\n';
            };

        };

private:
    std::mutex m;
    mongocxx::client client;
    mongocxx::collection collection;
    mongocxx::uri uri;
    static mongocxx::instance inst;



};


#endif //WS_TO_MONGO_MSG_HANDLER_H
