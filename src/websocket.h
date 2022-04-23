#ifndef WS_TO_MONGO_WEBSOCKET_H
#define WS_TO_MONGO_WEBSOCKET_H

#include "string"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;


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

void on_message (client* c,websocketpp::connection_hdl hdl, message_ptr msg) {
//    c->get_alog().write(websocketpp::log::alevel::app, "Received Reply: " + msg->get_payload());
    std::cout << "Received Reply: " + msg->get_payload() + "\n";
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
    Ws (std::string &w_a, std::string &p) {

        wss_address = w_a;
        params = p;

        m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
        m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

        m_endpoint.init_asio();
        m_endpoint.start_perpetual();

    }

    ~Ws() {
        std::cout << "Destroyed";
//        std::string reason = "kill";
//        m_endpoint.close(con->get_handle(), websocketpp::close::status::normal, reason);
    }

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
                websocketpp::lib::placeholders::_2
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

};

#endif //WS_TO_MONGO_WEBSOCKET_H
