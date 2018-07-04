#include <fstream>


#define BOOST_TEST_MODULE test_main

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(test_suite_main_1)

BOOST_AUTO_TEST_CASE(file_wr_thread_exetpion)
{
    std::system("chmod 755 ../../tests/file_wr_thread_exetpion.sh");
    std::system("../../tests/file_wr_thread_exetpion.sh");
 
    std::ifstream f("OK");
    BOOST_CHECK_EQUAL( f.good(), true );  

    std::system("rm -f bulk*.log error.log res OK");
}

BOOST_AUTO_TEST_SUITE_END()