#pragma once

#include <chrono>
#include <thread>
#include <future>

#ifdef _WINDOWS
    #include <windows.h>
#else
    #include <unistd.h>
    #define Sleep(x) usleep((x)*1000)
#endif

class Timer 
{
public:
    template <typename... ParamTypes>
    void setTimeOut(int milliseconds, std::function<void(ParamTypes...)> func, ParamTypes... parames)
    {
        // unset clear
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_clear = false;
            lk.unlock();
        }

        std::thread t([=]() {
            // sleep until timeout
            std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));

            // lock scope
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                if(m_clear)
                {
                    lk.unlock();
                    return;
                }
                lk.unlock();
            }

            // execute function
            func(parames...); 
        });

        t.detach();
    };

    template <typename... ParamTypes>
    void setInterval(int milliseconds, std::function<void(ParamTypes...)> func, ParamTypes... parames)
    {
        // unset clear
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_clear = false;
            lk.unlock();
        }
        
        std::thread t([=]() {
            while(true) 
            {
                // sleep until timeout
                std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));

                // lock scope
                {
                    std::unique_lock<std::mutex> lk(m_mutex);
                    if(m_clear)
                    {
                        lk.unlock();
                        return;
                    }
                    lk.unlock();
                }
                
                // execute function
                func(parames...);
            }
        });

        t.detach();
    };
    
    void stop()
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_clear = true;
        lk.unlock();
    }

private:
    bool m_clear = false;
    std::mutex m_mutex;
};