#include <queue>
#include <condition_variable>
#include <atomic>

template<typename T>
struct queue_wrapper
{

    void push( T&& value )
    {
        q_data.push(value);
    }


    void push( const T& value )
    {
        q_data.push(value);
    }


    T& front()
    {
        return q_data.front();
    }


    void pop(){

        return q_data.pop();
    }


    bool empty() const
    {
        return q_data.empty();
    }

    
    auto size() const
    {
        return q_data.size();
    }


    std::condition_variable cv;
    std::mutex cv_mx;

    std::condition_variable cv_empty;
    std::mutex cv_mx_empty;

    std::exception_ptr eptr {nullptr};

private:  
    std::queue<T> q_data;
};