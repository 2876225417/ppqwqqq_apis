



#include "apis/connection/connection.h"
#include "apis/router/router.h"
#include "apis/server/server.h"

#include "apis/db_utils/postgres_conn.h"
#include "apis/db_utils/conn_pool.h"
#include "apis/articles/article_repository.h"
#include "apis/users/user_management.h"

#include <vector>


int main() {
    try {
        asio::io_context ioc;
        postgres_conn_pool::instance().configure(
            "postgres",
            "postgres",
            "20041025",
            "localhost",
            5432,
            15
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

    postgres_conn_pool::instance().configure(
        "postgres",
        "postgres",
        "20041025",
        "localhost",
        5432,
        1
    );
    return 0;
}