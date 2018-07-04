#include "session_storage.h"
#include "bulk_session.h"
#include "bulk_server.h"
#include "functions.h"
#include "debug_options.h"
#include <boost/program_options.hpp>

size_t Command::file_id = 0;

namespace po = boost::program_options;
static int get_debug_options(int ac, char** av) 
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("exception,e", po::value<uint32_t>(), "set debug exception")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    if (vm.count("exception")) {
        debug_param::exep_opt = true;  
        debug_param::exep_cnt = vm["exception"].as<uint32_t>();    
    } else {
        debug_param::exep_opt = false;
    }
    return 0;
}


int main(int argc, char** argv)
{
    try {    
        if (argc < 3){
            std::cerr << "Usage: bulk_server <port> <bulk_size> [-e]" << std::endl;
            return 1;
        }
        else{
            if(get_debug_options(argc, argv))
                return 1;
        }

        process(argv[1], argv[2]);
    }
    catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
