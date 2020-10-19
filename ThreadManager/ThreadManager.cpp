#include "ThreadManager.hpp"


namespace Proxy
{

    ThreadManager::ThreadManager() noexcept
    {
        const auto threadNumber = std::thread::hardware_concurrency();

        for(int32_t i = 0; i < threadNumber; ++i)
        {
            std::thread newThread;
            availableThreadsIDs_.push_back(newThread.get_id());
            threads_.emplace_back(std::move(newThread));
        }
    }

    void ThreadManager::SetThreadEngaged(std::thread::id threadID) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex);
        for(auto it = availableThreadsIDs_.begin(); it != availableThreadsIDs_.end(); ++it)
        {
            if(*it == threadID)
            {
                availableThreadsIDs_.erase(it);
            }
        }
    }

    void ThreadManager::SetThreadAvailable(std::thread::id threadID) noexcept
    {
        std::lock_guard<std::mutex> lock(mutex);
        availableThreadsIDs_.push_front(threadID);
    }

    std::thread* ThreadManager::GetAvailableThread() const noexcept
    {

        const auto availableThreadID = availableThreadsIDs_.back();
        for(auto& it: threads_)
        {
            if(it.get_id() == availableThreadID)
            {
                return const_cast<std::thread*>(&it);
            }
        }
    }
}