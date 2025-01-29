#ifndef CONNECTION_H
#define CONNECTION_H

#include <thread>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <memory>
#include <unordered_map>
#include <iostream>

#include "../router/router.h"

namespace asio  = boost::asio;
namespace beast = boost::beast;
namespace http  = beast::http;

using tcp       = asio::ip::tcp;
using Request   = http::request<http::dynamic_body>;


class connection
    : public std::enable_shared_from_this<connection> {

public:
    connection(tcp::socket socket, const router& router)
        : m_socket(std::move(socket))
        , m_router(router) { }
        
    void start() {
        read_request();
    }



private:
    tcp::socket         m_socket;
    beast::flat_buffer  m_buffer;
    Request             m_request;
    Response            m_response;
    const router&       m_router;

    void read_request() {
        auto self = shared_from_this();
        
        http::async_read( m_socket
                        , m_buffer
                        , m_request
                        , [self](beast::error_code ec, size_t) {
                            if (!ec) 
                                self->process_request();
                        } );
    }

    void process_request() {
        m_response.version(m_request.version());
        m_response.keep_alive(false);

        if (!m_router.handle_request(m_request, m_response)) {
            m_response.result(http::status::not_found);
            m_response.set(http::field::content_type, "text/plain");
            m_response.body() = "404 Not Found!";
        }
        
        m_response.prepare_payload();
        write_response();
    }

    void write_response() {
        auto self = shared_from_this();
        
        http::async_write( m_socket
                         , m_response
                         , [self](beast::error_code ec, size_t) {
                            self->m_socket.shutdown(tcp::socket::shutdown_send, ec);
                         });
    }
};

#endif