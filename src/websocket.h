#pragma once
#ifndef WS_TO_MONGO_WEBSOCKET_H
#define WS_TO_MONGO_WEBSOCKET_H

#include <nlohmann/json.hpp>
#include "string"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include "msg_handler.h"

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;


using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using json = nlohmann::json;



context_ptr on_tls_init(const char * hostname, websocketpp::connection_hdl) {
    context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 |
                         boost::asio::ssl::context::single_dh_use);

        ctx->set_verify_mode(boost::asio::ssl::verify_none);
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return ctx;
}

void on_open(client* c,websocketpp::connection_hdl hdl, std::string &params){ //

    c->send(hdl, params, websocketpp::frame::opcode::text);
    std::cout << "Sent subscription params \n";
};

void on_message (client* c,websocketpp::connection_hdl hdl, message_ptr msg, std::vector<json> *input) {

    auto response = json::parse(msg->get_payload());
    std::cout << response << std::endl;
    bool cond1 = response.at("arg").at("channel").get<std::string>() == "candle1m";
    bool cond2 = response.find("event") == response.end();
    try {
        if (cond1 and cond2) {
            input->push_back(response);
        };
    }catch(json::out_of_range &e){
        std::cout << e.what() << std::endl;
    } ;
};

void on_fail(client* c,websocketpp::connection_hdl hdl) {
    c->get_alog().write(websocketpp::log::alevel::app, "Connection Failed");
}

void on_close(client* c, websocketpp::connection_hdl hdl) {
    c->get_alog().write(websocketpp::log::alevel::app, "Connection Closed");
    c->close(hdl, websocketpp::close::status::normal, "");

}

class Ws {
public:
    Ws (std::string &w_a, std::string &p, std::vector<json>* input_container) {

        wss_address = w_a;
        params = p;
        input = input_container;

        m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

        m_endpoint.init_asio();
        m_endpoint.start_perpetual();

    }

    ~Ws() {}

    void connect() {
        websocketpp::lib::error_code ec;

        m_endpoint.set_tls_init_handler(bind(&on_tls_init, wss_address.c_str(), ::_1));
        con = m_endpoint.get_connection(wss_address, ec);

        con->set_open_handler(websocketpp::lib::bind(
                on_open,
                &m_endpoint,
                websocketpp::lib::placeholders::_1,
                params
        ));
        con->set_fail_handler(websocketpp::lib::bind(
                on_fail,
                &m_endpoint,
                websocketpp::lib::placeholders::_1
        ));
        con->set_message_handler(websocketpp::lib::bind(
                on_message,
                &m_endpoint,
                websocketpp::lib::placeholders::_1,
                websocketpp::lib::placeholders::_2,
                input
        ));
        con->set_close_handler(websocketpp::lib::bind(
                on_close,
                &m_endpoint,
                websocketpp::lib::placeholders::_1
        ));

        m_endpoint.connect(con);
        m_endpoint.run();

    }

private:
    std::string wss_address;
    std::string params;
    client m_endpoint;
    client::connection_ptr con;
    std::vector<json> *input;

};

#endif //WS_TO_MONGO_WEBSOCKET_H
