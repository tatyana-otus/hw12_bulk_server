#include "consts_types.h"
#include "session_storage.h"
#include "bulk_session.h"
#include "bulk_server.h"
#include "functions.h"
#include "debug_options.h"

//using boost::asio::ip::tcp;

size_t Command::file_id = 0;

#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(test_suite_main_1)

BOOST_AUTO_TEST_CASE(invalid_bulk_size)
{
    BOOST_CHECK_THROW(process("7777", "-1"  ), std::invalid_argument);
    BOOST_CHECK_THROW(process("7777", "0"   ), std::invalid_argument);
    BOOST_CHECK_THROW(process("7777", "0x10"), std::invalid_argument);
    BOOST_CHECK_THROW(process("7777", "a34" ), std::invalid_argument);

    auto str_1 = std::to_string(MAX_BULK_SIZE + 1);
    BOOST_CHECK_THROW(process("7777", str_1.c_str()), std::invalid_argument);
}


BOOST_AUTO_TEST_CASE(invalid_port)
{  
    BOOST_CHECK_THROW(process("-1",    "1"), std::invalid_argument);
    BOOST_CHECK_THROW(process("as45",  "1"), std::invalid_argument);
    BOOST_CHECK_THROW(process("http",  "1"), std::invalid_argument);
    BOOST_CHECK_THROW(process("65536", "1"), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

