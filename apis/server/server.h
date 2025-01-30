

#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>


#include "../router/router.h"
#include "../connection/connection.h"
#include "../articles/article_repository.h"
#include "../users/user_management.h"
#include "../images/include/image_resolver.hpp"


using json      = nlohmann::json;


class server {
public:
    server( asio::io_context& ioc
          , unsigned short port
          , article_repository& repo
          , user_manager& user_mgr)
          : m_acceptor(ioc, { tcp::v4(), port })
          , m_repo(repo)
          , m_user_manager(user_mgr) {
        setup_routes();
        accept();    
    }

private:
    tcp::acceptor       m_acceptor;
    router              m_router;
    article_repository& m_repo;
    user_manager&       m_user_manager;
    

    void setup_routes() {
        m_router.add_route( http::verb::get
                          , "/hello"
                          , [](const Request&, Response& res) {
                                res.result(http::status::ok);
                                res.set(http::field::content_type, "text/plain");
                                res.body() = "Hello from C++!";
                          });

        m_router.add_route( http::verb::get
                          , "/images/{category}/{name}"
                          , [this](const Request& req, Response& res) {
                                std::string doc_root = "/home/ppqwqqq/wallpaper";
                                std::string request_path = std::string(req.target());
                                std::string relative_path = request_path.substr(8);
                                fs::path file_path = fs::path(doc_root) / relative_path;
                                std::cout << "file_path: " <<  file_path << std::endl;

                                if (!fs::exists(file_path)) {
                                    res.result(http::status::not_found);
                                    res.body() = "File not found: " + file_path.string();
                                    return;
                                }
                                
                                fs::path canonical_path, canonical_root;
                                try {
                                    canonical_path = fs::canonical(file_path);
                                    canonical_root = fs::canonical(doc_root);
                                } catch (const fs::filesystem_error& e) {
                                    throw std::runtime_error("Filesystem error: "  + std::string(e.what()));
                                }

                                if (canonical_path.string().find(canonical_root.string()) != 0) {
                                    res.result(http::status::forbidden);
                                    res.body() = "Access denied";
                                    return;
                                }

                                if (fs::exists(file_path) && fs::is_regular_file(file_path)) {
                                    try {
                                        image img = load_image_from_file(file_path.string());
                                        
                                        res.result(http::status::ok);
                                        res.set(http::field::content_type, img.get_MIME_type());
                                        res.set(http::field::content_length, std::to_string(img.data.size()));
                                        res.set(http::field::access_control_allow_origin, "*");
                                        res.set(http::field::access_control_allow_methods, "GET, POST, OPTIONS");
                                        res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
                                        res.body() = std::string(img.data.begin(), img.data.end());
                                    } catch (const std::exception& e) {
                                        std::cout << "Error: " << e.what() << std::endl;
                                    }
                                }
                            });
        
        m_router.add_route( http::verb::post
                          , "/register"
                          , [this](const Request& req, Response& res) {
                                try {
                                    auto body = boost::beast::buffers_to_string(req.body().data());
                                    auto j = json::parse(body);

                                    std::string user_name = j["username"];
                                    std::string user_password = j["password"];

                                    auto user_info_opt = m_user_manager.user_register(user_name, user_password);
                                    if (user_info_opt) {
                                        const auto& user_info = user_info_opt.value();

                                        json reponse = {
                                            {"user_id", user_info.user_id},
                                            {"username", user_info.user_name},
                                        };

                                        res.result(http::status::ok);
                                        res.set(http::field::content_type, "application/json");
                                        res.body() = reponse.dump();
                                    } else {
                                        res.result(http::status::conflict);
                                        res.set(http::field::content_type, "application/json");
                                        res.body() = json{{"error", "User registerred failed!"}}.dump();
                                    }
                                } catch (const std::exception& e) {
                                    res.result(http::status::internal_server_error);
                                    res.set(http::field::content_type, "application/json");
                                    res.body() = json{{"error", e.what()}}.dump();
                                }
                          });

        m_router.add_route( http::verb::post
                          , "/login"
                          , [this](const Request& req, Response& res) {
                                try {
                                    auto body = boost::beast::buffers_to_string(req.body().data());
                                    auto j = json::parse(body);

                                    std::string user_name = j["username"];
                                    std::string user_password = j["password"];

                                    auto user_info_opt = m_user_manager.user_login(user_name, user_password);

                                    if (user_info_opt) {
                                        const auto& user_info = user_info_opt.value();
                                        
                                        json response = {
                                            {"user_id", user_info.user_id},
                                            {"username", user_info.user_name}
                                        };
                                        std::cout << "Good!!!" << std::endl;

                                        res.result(http::status::ok);
                                        res.set(http::field::content_type, "application/json");
                                        res.body() = response.dump();
                                    } else {
                                        res.result(http::status::conflict);
                                        res.set(http::field::content_type, "application/json");
                                        res.body() = json{{"error", "User logged in failed!!!"}}.dump();
                                    }
                                } catch (const std::exception& e) {
                                    res.result(http::status::internal_server_error);
                                    res.set(http::field::content_type, "application/type");
                                    res.body() = json{{"error", e.what()}}.dump();
                                }
                          } );

        
        m_router.add_route( http::verb::get
                          , "/articles/{id}"
                          , [this](const Request& req, Response& res) {
                                try {
                                    int id = std::stoi(std::string(req.target()).substr(std::string(req.target()).find_last_of('/') + 1));
                                    if (auto article = m_repo.get_article_by_id(id)) {
                                        json j = json::object({
                                            {"id", article->id},
                                            {"title", article->title},
                                            {"author", article->author},
                                            {"created_at", article->created_at.time_since_epoch().count()},
                                            {"updated_at", article->updated_at.time_since_epoch().count()},
                                            {"published", article->published}
                                        });
                                        res.body() = j.dump();
                                        res.result(http::status::ok);
                                    } else {
                                        res.result(http::status::not_found);
                                        res.body() = json{{"error", "Article not found"}}.dump();
                                    }
                                    res.set(http::field::content_type, "application/json");
                                    res.set(http::field::access_control_allow_origin, "*");
                                    res.set(http::field::access_control_allow_methods, "GET, POST, OPTIONS");
                                    res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
                                } catch (const std::exception& e) {
                                    res.result(http::status::bad_request);
                                    res.body() = json{{"error", e.what()}}.dump();
                                }
                          });
    
        m_router.add_route( http::verb::get
                          , "/articles"
                          , [this](const Request&, Response& res) {
                                try {
                                    auto articles = m_repo.get_all_published_aritcles();
                                    json j = json::array();
                                    for (const auto& a: articles) {
                                        j.push_back({
                                            {"id", a.id},
                                            {"title", a.title},
                                            {"author", a.author},
                                            {"created_at", a.created_at.time_since_epoch().count()},
                                            {"updated_at", a.updated_at.time_since_epoch().count()}
                                        });
                                    }
                                    
                                    res.body() = j.dump();
                                    res.set(http::field::content_type, "application/json");
                                    res.set(http::field::access_control_allow_origin, "*");
                                    res.set(http::field::access_control_allow_methods, "GET, POST, OPTIONS");
                                    res.set(http::field::access_control_allow_headers, "Content-Type, Authorization");
                                    res.result(http::status::ok);
                                } catch (const std::exception& e) {
                                    res.result(http::status::internal_server_error);
                                    res.body() = json{{"error", e.what()}}.dump();
                                }
                          } );
    }

    void accept() {
        m_acceptor.async_accept(
            [this](beast::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<connection>( std::move(socket)
                                                , m_router)->start();
                }
                accept();
            });
    }
};

#endif