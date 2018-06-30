#include <cstdlib>
#include <iostream>
#include <memory>
#include <set>
#include <map>
#include <utility>
#include <boost/asio.hpp>

class bulk_server
{
public:
    bulk_server(boost::asio::io_service& io_service,
      const tcp::endpoint& endpoint, size_t bulk_size_, std::ostream& os)
    : acceptor(io_service, endpoint),
      socket(io_service), 
      bulk_size(bulk_size_)
    {
        storage = std::make_shared<session_storage>(bulk_size, os);
        storage->start();
        do_accept();
    }

    ~bulk_server()
    {
        try {  
            storage->stop();
        }
        catch(const std::exception &e) {

        }

    }

private:
    void do_accept()
    {
        acceptor.async_accept(socket,
            [this](boost::system::error_code ec)
            {
              if (!ec)
            {
                std::make_shared<bulk_session>(std::move(socket), storage)->start();
            }
            else{
                std::cout << "do_accept() - error !\n";
            }

              do_accept();
            });
    }

    tcp::acceptor acceptor;
    tcp::socket socket;
    std::shared_ptr<session_storage> storage;
    size_t bulk_size;
};