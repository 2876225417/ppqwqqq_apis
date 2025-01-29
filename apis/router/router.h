
#ifndef ROUTER_H
#define ROUTER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <unordered_map>
#include <regex>
#include <functional>


namespace asio  = boost::asio;
namespace beast = boost::beast;
namespace http  = beast::http;
namespace fs    = std::filesystem;

using tcp       = asio::ip::tcp;
using Request   = http::request<http::dynamic_body>;
using Response  = http::response<http::string_body>;

class router {
public:
    using Handler = std::function<void(const Request&, Response&)>;

    void add_route( http::verb method
                  , std::string path
                  , Handler handler
                  ) {
        if (path.find("{") != std::string::npos) 
            path = convert_to_regex(path);
        std::cout << "path: " << path << std::endl;
        m_routes[path][method] = std::move(handler);
    }

    bool handle_request(const Request& req, Response& res) const {
        auto path = std::string(req.target());

        for (const auto& route: m_routes) {
            if (std::regex_match(path, std::regex(route.first))) {
                const auto& method_map = route.second;
                auto method_it = method_map.find(req.method());
                if (method_it == method_map.end()) {
                    res.result(http::status::method_not_allowed);
                    return true;
                }
                method_it->second(req, res);
                return true;
            }
         }
         
        return false;
    }
    
private:
    std::unordered_map< std::string
                      , std::unordered_map<http::verb, Handler>
                      > m_routes;

    std::string convert_to_regex(const std::string& path) const {
        std::string regex = path;
        std::regex dynamic_pattern(R"(\{[^}]*\})");
        regex = std::regex_replace(regex, dynamic_pattern, "[^/]+");
        return "^" + regex + "$";
    }
};

#endif