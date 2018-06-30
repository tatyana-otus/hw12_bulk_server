#include <cstdlib>
#include <iostream>
#include <memory>
#include <set>
#include <map>
#include <utility>
#include <boost/asio.hpp>
#include "consts_types.h"


class bulk_session:
public std::enable_shared_from_this<bulk_session>
{
public:

    bulk_session(tcp::socket socket_, std::shared_ptr<session_storage> storage_)
    : socket(std::move(socket_)), ss(storage_)
    {
        cmd.reserve(MAX_CMD_LENGTH);
    }


    void start ()
    {
        ss->add_session(shared_from_this());
        do_read_char();  
    }

private:

  void do_read_char()
  {
    auto self(shared_from_this());
    boost::asio::async_read(socket,
        boost::asio::buffer(&data, 1),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec) {
                if(data != '\n') {
                    cmd += data;
                }
                else{
                    ss->add_cmd(shared_from_this(), cmd);
                    cmd = "";
                }
                do_read_char();
            }
            else{
                socket.close();
                ss->remove_session(shared_from_this());
            }
        });
    }

    tcp::socket socket;
    char data;
    std::shared_ptr<session_storage> ss;
    std::string cmd;
};