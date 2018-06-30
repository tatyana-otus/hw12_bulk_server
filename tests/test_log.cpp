#include "consts_types.h"
#include "session_storage.h"
#include "bulk_session.h"
#include "bulk_server.h"
#include "functions.h"
#include <chrono> 

using boost::asio::ip::tcp;

size_t Command::file_id = 0;

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(test_suite_1)

BOOST_AUTO_TEST_CASE(create_log_file)
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    auto main_th_id = ss.str();

    std::size_t bulk_size = 1;
    std::time_t init_time = std::time(nullptr);

    std::stringstream oss;
    {
        boost::asio::io_service io_service;
        tcp::endpoint endpoint(tcp::v4(), 9001);
        bulk_server ser(io_service, endpoint, bulk_size, oss);

        std::thread server_th([&io_service](){ io_service.run(); });
        std::thread client_th([](){ std::system("seq 1 10 | nc localhost 9001"); });

        client_th.join();
        io_service.stop();
        server_th.join();
    }    

    const std::size_t files_number {10};
    for(size_t i = 0; i < files_number; ++i) {

        size_t id = i + 1;
        int watchdog_sec = 0;
        std::string name;
        do{
            if(watchdog_sec != 0) ++init_time;
            name = "bulk" + std::to_string(init_time) + "_" 
                          + main_th_id + "_" 
                          + std::to_string(id) + ".log";
            ++watchdog_sec;
        }
        while(!(std::ifstream{name}) && (watchdog_sec < 10));
        BOOST_CHECK(watchdog_sec < 10);

        std::ifstream log_file {name};
        BOOST_CHECK_EQUAL( log_file.good(), true );

        std::stringstream log_buff;
        log_buff << log_file.rdbuf();
        log_file.close();

        BOOST_CHECK_EQUAL( log_buff.str(), "bulk: " + std::to_string(i + 1) );
    }    
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_2)

BOOST_AUTO_TEST_CASE(log_file_alredy_exist)
{
    std::stringstream ss;
    std::stringstream oss;
    ss << std::this_thread::get_id();
    auto main_th_id = ss.str();

    std::size_t bulk_size = 1;

    std::time_t init_time = std::time(nullptr);
    
    int count = 10;
    while(--count) {
        std::string f_name = "bulk" + std::to_string(init_time++) + "_" 
                            + main_th_id + "_1" 
                            + ".log";
        std::ofstream of{f_name};
        BOOST_CHECK_EQUAL( of.good(), true );
        of.close();
    }    


    {
        std::thread client_th([](){ 
                                    std::this_thread::sleep_for(std::chrono::seconds(1));
                                    std::system("seq 1 100 | nc localhost 9001"); 
                                  });
        client_th.detach();

        BOOST_CHECK_THROW(process("9001", "1"  ), std::logic_error);

    }
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(test_suite_delete)

BOOST_AUTO_TEST_CASE(remove_test_files)
{
    std::system("rm -f bulk*.log");
    std::system("rm -f res*.txt");
}

BOOST_AUTO_TEST_SUITE_END()