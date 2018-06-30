//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

/*#include <cstdlib>
//#include <deque>
#include <iostream>
//#include <list>
#include <memory>
#include <set>
#include <map>
#include <utility>
#include <boost/asio.hpp>

//#include <functional>
#include <shared_mutex>
#include "command.h"
#include "bulk_handlers.h"*/


#include "session_storage.h"
#include "bulk_session.h"
#include "bulk_server.h"
#include "functions.h"

using boost::asio::ip::tcp;

size_t Command::file_id = 0;
//----------------------------------------------------------------------
/*class bulk_session;

class session_storage
{   
public:
    std::string fail_msg = "async lib failed\n";

    session_storage(size_t bulk_size_, std::ostream& os = std::cout, 
                size_t file_th_cnt_ = 2):bulk_size(bulk_size_), file_th_cnt(file_th_cnt_)
    {
        std::stringstream ss;
        ss << std::this_thread::get_id();
        auto main_th_id = ss.str();

        q_file  = std::make_shared<f_tasks_t>();
        q_print = std::make_shared<p_tasks_t>();

        data_log = std::make_shared<PrintData>(q_print, os); 
        file_log.resize(file_th_cnt);
        for(size_t i = 0; i < file_th_cnt; ++i){
            file_log[i] = std::make_shared<WriteData>(q_file, 
                                                      main_th_id);
        }

        static_cmd = std::make_unique<Command>(bulk_size,
                                                q_file, 
                                                q_print 
                                               );

        start();
    }


    ~session_storage()
    {
        try {  
            stop();
        }
        catch(const std::exception &e) {

        }       
    }

    public:
    void add_session(std::shared_ptr<bulk_session> s) 
    {
        //sessions.insert(s);
        sessions[s] = std::make_unique<Command>(bulk_size,
                                                q_file, 
                                                q_print 
                                                );
    };

    void remove_session(std::shared_ptr<bulk_session> s)
    {
        sessions.erase(s);
    };


    void add_cmd(std::shared_ptr<bulk_session> s, const std::string cmd)
    {
        auto it = sessions.find(s);
        if(it != sessions.end()) {
            if( (cmd == "{") || (cmd == "}") || (it->second->is_static() != true) )
                it->second->get_data(std::move(cmd));
            else
                static_cmd->get_data(std::move(cmd));
        }
    };

private:

    std::unique_ptr<Command> static_cmd;
    std::map<std::shared_ptr<bulk_session>, std::unique_ptr<Command>> sessions;


    std::atomic<bool> fail{false};

    void start()
    {
        data_log->start();
        for (auto const& f : file_log) {
            f->start();
        }
    }


    void stop()
    {
        data_log->quit = true;
        for (auto & f : file_log) {
            f->quit = true;
        }

        q_print->cv.notify_one();
        q_file->cv.notify_all();

        data_log->stop();
        for (auto const& f : file_log) {
            f->stop();
        }
    }
    size_t bulk_size;
    size_t file_th_cnt;
    
    std::shared_ptr<PrintData> data_log;
    std::vector<std::shared_ptr<WriteData>> file_log;

    std::shared_ptr<f_tasks_t> q_file;
    std::shared_ptr<p_tasks_t> q_print;
};
*/
/*
class bulk_session:
public std::enable_shared_from_this<bulk_session>
{
public:

    bulk_session(tcp::socket socket_, std::shared_ptr<session_storage> storage_)
    : socket(std::move(socket_)), ss(storage_)
    {
        cmd.reserve(MAX_CMD_LENGTH);
    }

    
    ~bulk_session()
    {
        socket.close();
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
        [this, self](boost::system::error_code ec, std::size_t )
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
                std::cout << "do_read_char() - error !\n";
                ss->remove_session(shared_from_this());
            }
        });
    }

    tcp::socket socket;
    char data;
    //session_storage& ss;
    std::shared_ptr<session_storage> ss;
    std::string cmd;
};
*/

//----------------------------------------------------------------------
/*
class bulk_server : std::enable_shared_from_this<bulk_server>
{
public:
    bulk_server(boost::asio::io_service& io_service,
      const tcp::endpoint& endpoint, size_t bulk_size_)
    : acceptor(io_service, endpoint),
      socket(io_service),
      bulk_size(bulk_size_)
    {
        storage = std::make_shared<session_storage>(bulk_size);
        do_accept();
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
    //session_storage storage;
    size_t bulk_size;
};
*/
//----------------------------------------------------------------------

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

 /* try
  {
    if (argc != 3)
    {
      std::cerr << "bulk_server <port> <bulk_size>\n";
      return 1;
    }

    //check for digits argv[1] argv[2]


    boost::asio::io_service io_service;

    boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
    signals.async_wait(
    std::bind(&boost::asio::io_service::stop, &io_service));

    tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));

    bulk_server server(io_service, endpoint, std::atoi(argv[2]));

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;*/
}
