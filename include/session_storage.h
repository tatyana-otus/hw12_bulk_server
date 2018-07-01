#include <iostream>
#include <map>
#include <boost/asio.hpp>

#include "command.h"
#include "bulk_handlers.h"

using boost::asio::ip::tcp;

class bulk_session;

class session_storage
{   
public:

    session_storage(size_t bulk_size_, std::ostream& os, 
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
    }


    public:
    void add_session(std::shared_ptr<bulk_session> s) 
    {
        sessions[s] = std::make_unique<Command>(bulk_size,
                                                q_file, 
                                                q_print 
                                                );
    };

    void remove_session(std::shared_ptr<bulk_session> s)
    {
        sessions.erase(s);
    };


    void add_cmd(std::shared_ptr<bulk_session> s, const std::string& cmd)
    {
        auto it = sessions.find(s);
        if(it != sessions.end()) {
            if( (cmd == "{") || (cmd == "}") || (it->second->is_static() != true) )
                it->second->get_data(cmd);
            else
                static_cmd->get_data(cmd);
        }
    };

    void stop()
    {
        static_cmd->on_cmd_end();

        std::unique_lock<std::mutex> lk(q_print->cv_mx);
        data_log->quit = true;
        q_print->cv.notify_one();
        lk.unlock();

        std::unique_lock<std::mutex> lk_file(q_file->cv_mx);
        for (auto & f : file_log) {
            f->quit = true;
        }
        q_file->cv.notify_all();
        lk_file.unlock();

        data_log->stop();
        for (auto const& f : file_log) {
            f->stop();
        }
    }

    void start()
    {
        data_log->start();
        for (auto const& f : file_log) {
            f->start();
        }
    }

private:
    using cmd_t = std::unique_ptr<Command>;
    
    cmd_t static_cmd;
    std::map<std::shared_ptr<bulk_session>, cmd_t> sessions;
    
    size_t bulk_size;
    size_t file_th_cnt;
    
    std::shared_ptr<PrintData> data_log;
    std::vector<std::shared_ptr<WriteData>> file_log;

    std::shared_ptr<f_tasks_t> q_file;
    std::shared_ptr<p_tasks_t> q_print;
};