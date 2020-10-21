#ifndef PROXYTCP_THREADPOOL_HPP
#define PROXYTCP_THREADPOOL_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <iostream>
#include <functional>
#include "Utilities/NonCopyable.hpp"


namespace Proxy
{
    template<typename WorkType>
    class ThreadPool
    {
    public:
        ThreadPool() noexcept
        {
            const auto availableThreads = std::thread::hardware_concurrency();

            workers_.reserve(availableThreads);
            for(int32_t i = 0; i < availableThreads; ++i)
            {
                workers_.emplace_back([this]() {
                    while(true)
                    {
                        std::function<void()> task;

                        PopTaskFromQueue(task);

                        task();

                    }
                });
            }
        }

        ~ThreadPool() noexcept
        {
            workConditional_.notify_all();
            for(auto& it : workers_)
            {
                std::cout << "[Thread " << std::this_thread::get_id() << " is joined]\n";
                it.join();
            }
        }

        ThreadPool(ThreadPool const& rhs) noexcept
        {
            std::lock_guard<std::mutex> lk(rhs.mut_);
            workQueue_=rhs.workQueue_;
        }

    public:
        void AddWork(WorkType newWork) noexcept
        {
            std::lock_guard<std::mutex> lk(mut_);
            workQueue_.push(newWork);
            workConditional_.notify_one();
        }

        void PopTaskFromQueue(WorkType& task) noexcept
        {
            std::unique_lock<std::mutex> lk(mut_);
            workConditional_.wait(lk, [this]{return !workQueue_.empty();});
            task = workQueue_.front();
            workQueue_.pop();
        }

        bool empty() const noexcept
        {
            std::lock_guard<std::mutex> lk(mut_);
            return workQueue_.empty();
        }

    private:
        mutable std::mutex mut_;
        std::queue<WorkType> workQueue_;
        std::vector<std::thread> workers_;
        std::condition_variable workConditional_;
    };



}

#endif //PROXYTCP_THREADPOOL_HPP
