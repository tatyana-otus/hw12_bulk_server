project(test)

find_package(Boost COMPONENTS system unit_test_framework REQUIRED)
find_package(Threads)


add_executable(test_server_start_cli   test_server_start.cpp)
add_executable(test_log_cli            test_log.cpp)
add_executable(test_bulk_cli           test_bulk.cpp)


target_include_directories(test_server_start_cli  PRIVATE ../include)
target_include_directories(test_server_start_cli  PRIVATE ${Boost_INCLUDE_DIR})

target_include_directories(test_log_cli PRIVATE ../include)
target_include_directories(test_log_cli PRIVATE ${Boost_INCLUDE_DIR})

target_include_directories(test_bulk_cli PRIVATE ../include)
target_include_directories(test_bulk_cli PRIVATE ${Boost_INCLUDE_DIR})

set_target_properties(test_server_start_cli test_bulk_cli test_log_cli PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
  COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
)
#set (CMAKE_CXX_FLAGS "-fsanitize=thread")

target_link_libraries(test_server_start_cli
  ${CMAKE_THREAD_LIBS_INIT} 
  ${Boost_LIBRARIES}
)

 target_link_libraries(test_log_cli
   ${CMAKE_THREAD_LIBS_INIT}
   ${Boost_LIBRARIES}
)

 target_link_libraries(test_bulk_cli
   ${CMAKE_THREAD_LIBS_INIT}
   ${Boost_LIBRARIES}
 )

enable_testing()

add_test(test_start          test_server_start_cli  -t test_suite_main_1)

add_test(serial_static       test_bulk_cli  -t static_bulk_1)
add_test(async_static        test_bulk_cli  -t static_bulk_2)
add_test(serial_dinamic      test_bulk_cli  -t dinamic_bulk_1)
add_test(async_dinamic       test_bulk_cli  -t dinamic_bulk_2)

add_test(create_log_file       test_log_cli   -t test_suite_1)
add_test(log_file_alredy_exist test_log_cli   -t test_suite_2)

add_test(test_suite_delete     test_log_cli   -t test_suite_delete)