#include "consts_types.h"
#include "session_storage.h"
#include "bulk_session.h"
#include "bulk_server.h"


using boost::asio::ip::tcp;

size_t Command::file_id = 0;

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>

static void run_serial_th(std::vector<std::thread>& th, std::function<void()> f)
{
    for(auto & t : th){
        t = std::thread(f);
        t.join();
    }
}


static void run_async_th(std::vector<std::thread>& th, std::function<void()> f)
{
    for(auto & t : th){
        t = std::thread(f);
    }
    for(auto & t : th){
        t.join();
    }
}

BOOST_AUTO_TEST_SUITE(static_bulk_1)

BOOST_AUTO_TEST_CASE(serial_threads_client)
{  
    std::string data = 
        "bulk: 1, 2, 3\n"
        "bulk: 4, 5, 6\n"
        "bulk: 7, 8, 9\n"
        "bulk: 10, 1, 2\n"
        "bulk: 3, 4, 5\n"
        "bulk: 6, 7, 8\n"
        "bulk: 9, 10\n";
    
    std::stringstream oss;
    {
        const size_t client_th_num = 2;
        std::vector<std::thread> client_th;
        client_th.resize(client_th_num);
        auto client = [](){ std::system("seq 1 10 | nc localhost 9001"); };

        boost::asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), 9001);
        bulk_server ser(io_service, endpoint, 3, oss);

        std::thread server_th([&io_service](){ io_service.run(); });
        run_serial_th(client_th, client);


        io_service.stop();
        server_th.join();
    }    
    BOOST_CHECK_EQUAL( oss.str(), data);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(static_bulk_2)

BOOST_AUTO_TEST_CASE(async_threads_client)
{  
    const size_t repeat = 40;
    const size_t client_th_num = 100;
    std::stringstream oss;

    {
        std::vector<std::thread> client_th;
        client_th.resize(client_th_num);

        boost::asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), 9001);
        bulk_server ser(io_service, endpoint, 3, oss); //3

        std::thread server_th([&io_service](){ io_service.run(); });

        auto client = [repeat](){ 
                            for(size_t i = 0; i < repeat; ++i){
                                std::system("seq 1 4 | nc  localhost 9001"); //4
                             }   
                          };

        run_async_th(client_th, client);

        io_service.stop();
        server_th.join();
    }

    auto cnt = std::count(std::istreambuf_iterator<char>(oss), 
             std::istreambuf_iterator<char>(), '\n');

    auto rem = (4 * repeat * client_th_num ) % 3;
    BOOST_CHECK_EQUAL( cnt, (4 * repeat * client_th_num ) / 3 + ((rem == 0) ? 0 : 1));
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(dinamic_bulk_1)

BOOST_AUTO_TEST_CASE(serial_threads_client)
{  
    std::string data = 
        "bulk: 1, 2, 3, 4\n"
        "bulk: 1, 2, 3, 4\n"
        "bulk: 5, 6, 5\n"
        "bulk: 1, 2, 3, 4\n"
        "bulk: 6, 5, 6\n";
    
    std::stringstream oss;

    const size_t client_th_num = 3;
{    
    std::vector<std::thread> client_th;
    client_th.resize(client_th_num); 

    boost::asio::io_service io_service;
    tcp::endpoint endpoint(tcp::v4(), 9001);
    bulk_server ser(io_service, endpoint, 3, oss);

    std::thread server_th([&io_service](){ io_service.run(); });

    auto client = [](){ std::system("printf \"{\n1\n2\n3\n4\n}\n5\n6\n{\n7\n8\n9\n\" | nc  localhost 9001"); };

    run_serial_th(client_th, client);

    io_service.stop();
    server_th.join();
}

    BOOST_CHECK_EQUAL( oss.str(), data);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(dinamic_bulk_2)

BOOST_AUTO_TEST_CASE(async_threads_client)
{  
    std::string data = 
        "bulk: 1, 2, 3, 4\n";

    const size_t repeat = 100;
    const size_t client_th_num = 3;
    {
        std::vector<std::thread> client_th;
        client_th.resize(client_th_num);

        std::ofstream ofs("res.txt");
        BOOST_CHECK_EQUAL(ofs.good(), true);

        boost::asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), 9001);
        bulk_server ser(io_service, endpoint, 2, ofs);

        std::thread server_th([&io_service](){ io_service.run(); });

        auto client = [repeat](){ 
                            for(size_t i = 0; i < repeat; ++i){
                                std::system("printf \"{\n1\n2\n3\n4\n}\n{\n7\n8\n\" | nc  localhost 9001"); 
                             }   
                          };

        run_async_th(client_th, client);

        io_service.stop();
        server_th.join();
    }    
    
    std::system("sort res.txt | uniq | grep bulk > res_bulk.txt");

    auto file1 = std::ifstream("res_bulk.txt");
    std::stringstream ss1;
    ss1 << file1.rdbuf();
    BOOST_CHECK_EQUAL(ss1.str(), data);
    file1.close();

    auto file2 = std::ifstream("res.txt");
    std::stringstream ss2;
    ss2 << file2.rdbuf();
    auto cnt = std::count(std::istreambuf_iterator<char>(ss2), 
               std::istreambuf_iterator<char>(), '\n');
    BOOST_CHECK_EQUAL( cnt, client_th_num * repeat);
    file2.close();
}

BOOST_AUTO_TEST_SUITE_END()