#include <iostream>

using boost::asio::ip::tcp;


void process(const char* port_arg, const char* bulk_size_arg, std::ostream& os = std::cout)
{  
    std::string msg = "Invalid block size. Block size must be > 0  and < " + std::to_string(MAX_BULK_SIZE) + ".\n";

    std::string str_bulk_size = bulk_size_arg;
    if(!std::all_of(str_bulk_size.begin(), str_bulk_size.end(), ::isdigit))
        throw std::invalid_argument(msg.c_str());

    std::string str_port = port_arg;
    if(!std::all_of(str_port.begin(), str_port.end(), ::isdigit))
        throw std::invalid_argument("Invalid <port>");
    
    decltype(std::stoull("")) N ;
    decltype(std::stoull("")) port;
    try {
        port = std::stoull(str_port);
    }    
    catch(const std::exception &e) {
        throw std::invalid_argument("Invalid <port>");
    } 
    try {     
        N = std::stoull(str_bulk_size);
    }  
    catch(const std::exception &e) {
        throw std::invalid_argument(msg.c_str());
    }  

    if((N == 0) || (N > MAX_BULK_SIZE)){       
        throw std::invalid_argument(msg.c_str());
    }

    if(port > 65535){       
        throw std::invalid_argument("Invalid <port>");
    }

    boost::asio::io_service io_service;

    boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
    signals.async_wait(
    std::bind(&boost::asio::io_service::stop, &io_service));

    tcp::endpoint endpoint(tcp::v4(), port);

    bulk_server server(io_service, endpoint, N, os);
  
    io_service.run();
}