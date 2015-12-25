#include "executor.h"

#include <string>

Executor::Executor(size_t nThreads)
    : nThreads(nThreads)
{
    taskQueues.resize(nThreads);
    for (size_t i = 0; i < nThreads; i++)
        thread_pool.emplace_back([this, i] { run(i); });
}

Executor::~Executor()
{
    stop();
}

void Executor::stop()
{
    if (stopped)
        return;
    stopped = true;
    for (auto &q : taskQueues)
        q.done();
    for (auto &t : thread_pool)
        t.join();
}

void Executor::run(size_t i)
{
    while (1)
    {
        std::string error;
        try
        {
                Task t;
                const size_t spin_count = nThreads * 4;
                for (auto n = 0; n != spin_count; ++n)
                {
                    if (taskQueues[(i + n) % nThreads].try_pop(t))
                        break;
                }
                if (!t && !taskQueues[i].pop(t))
                    break;
                t();
        }
        catch (const std::exception &e)
        {
            error = e.what();
        }
        catch (...)
        {
            error = "unknown exception";
        }
        if (!error.empty())
        {
            printf("executor: %x, thread #%d, error: \n", this, i + 1, error.c_str());
        }
    }
}

Executor &getExecutor()
{
    static Executor executor;
    return executor;
}
