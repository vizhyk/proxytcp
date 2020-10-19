#ifndef PROXYTCP_THREADMANAGER_HPP
#define PROXYTCP_THREADMANAGER_HPP

#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include "Utilities/NonCopyable.hpp"


namespace Proxy
{
    class ThreadManager : NonCopyable
    {
    public:
        ThreadManager() noexcept;
        ~ThreadManager() noexcept = default;

        std::thread* GetAvailableThread() const noexcept;

        void SetThreadEngaged(std::thread::id threadID) noexcept;
        void SetThreadAvailable(std::thread::id threadID) noexcept;
    private:
        std::vector<std::thread> threads_;
        std::list<std::thread::id> availableThreadsIDs_;
        mutable std::mutex mutex;
    };

}

#endif //PROXYTCP_THREADMANAGER_HPP
