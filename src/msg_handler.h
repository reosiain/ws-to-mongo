#pragma once
#ifndef WS_TO_MONGO_MSG_HANDLER_H
#define WS_TO_MONGO_MSG_HANDLER_H


#include <mutex>
#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include "websocket.h"


using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

using json = nlohmann::json;

class MongoPusher{
public:
    explicit MongoPusher (std::string &db_url){

        mongocxx::uri uri(db_url);
        mongocxx::client client(uri);

        db = client["mydb"];
        coll = db["test"];

    }

    void push_to_db(std::vector<json> &output_container){

        std::vector<bsoncxx::document::value> docs;
        for(json elem : output_container) {

            auto id = elem["args"]["instId"].get<std::string>();
            auto dt = elem["data"][0][0].get<unsigned long long int>();
            auto o = elem["data"][0][1].get<float>();
            auto h = elem["data"][0][2].get<float>();
            auto l = elem["data"][0][3].get<float>();
            auto c = elem["data"][0][4].get<float>();
            auto vol_count = elem["data"][0][5].get<float>();
            auto vol_coin = elem["data"][0][6].get<float>();

            auto d = document{} << "instId" << id
                                  << "dt" << dt
                                  << "o" << o
                                  << "h" << h
                                  << "l" << l
                                  << "c" << c
                                  << "vol_count" << vol_count
                                  << "vol_coin" << vol_coin << finalize;

            docs.push_back(d);
        };

        coll.insert_many(docs);
        output_container.clear();
    };

    void monitor(std::vector<json> &input_container, std::vector<json> &output_container){

        try{
            while(true) {

                if (input_container.size() == 10) {
                    m.lock();
                    std::move(input_container.begin(), input_container.end(), output_container.begin());
                    m.unlock();
                    push_to_db(output_container);
                };
            };

        }catch(...) {
                std::cout << "Unexpected error" << std::endl;
            };

        };

private:
    std::mutex m;
    mongocxx::collection coll;
    mongocxx::database db;


};


#endif //WS_TO_MONGO_MSG_HANDLER_H
