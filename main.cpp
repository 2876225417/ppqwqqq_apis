



#include "apis/connection/connection.h"
#include "apis/router/router.h"
#include "apis/server/server.h"

#include "apis/db_utils/postgres_conn.h"
#include "apis/db_utils/conn_pool.h"
#include "apis/articles/article_repository.h"
#include "apis/users/user_management.h"

#include <vector>


int main(int argc, char* argv[]) {
    try {

        if (argc < 4) {
            std::cerr << "Usage: " << argv[0] << " <db_name> <db_user> <db_password> [db_host] [db_port] [pool_size]\n";
            std::cerr << "Defaults: db_host=localhost, db_port=5432, pool_size=15\n"; 
            return 1;
        }

        std::string db_name     = argv[1];
        std::string db_user     = argv[2];
        std::string db_password = argv[3];
        std::string db_host     = (argc > 4) ? argv[4] : std::string{"localhost"};
        int db_port             = (argc > 5) ? std::stoi(argv[5]) : int{5432};
        int db_conn_pool_size   = (argc > 6) ? std::stoi(argv[6]) : int{15};

        asio::io_context ioc;
        postgres_conn_pool::instance().configure(
            db_name,
            db_user,
            db_password,
            db_host,
            db_port,
            db_conn_pool_size
        );
        user_manager usr_mgr(postgres_conn_pool::instance());
        article_repository repo(postgres_conn_pool::instance());

        server server(ioc, 8080, repo, usr_mgr);
        const int thread_count = 4;
        std::vector<std::thread> threads;
        threads.reserve(thread_count);
        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&ioc] { ioc.run(); });
        }

        std::cout << "Server runing on port 8080..." << std::endl;
        for (auto& t: threads) {
            t.join();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}