#include "session_storage.h"
#include "bulk_session.h"
#include "bulk_server.h"
#include "functions.h"

using boost::asio::ip::tcp;

size_t Command::file_id = 0;

int main(int argc, char** argv)
{
    if (argc != 3) {        
        std::cerr << "Usage: bulk_server <port> <bulk_size>" << std::endl;
        return 1;
    }
    
    try {      
        process(argv[1], argv[2]);
    }
    catch(const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
