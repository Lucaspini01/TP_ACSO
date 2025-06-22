/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include "thread-pool.h"
using namespace std;

ThreadPool::ThreadPool(size_t numThreads) : wts(numThreads), done(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        wts[i].ts = thread([this, i] { worker(i); });
    }
    dt = thread([this] { dispatcher(); });
}

void ThreadPool::schedule(const function<void(void)>& thunk) {
    if (!thunk) throw invalid_argument("Cannot schedule a null function");

    {
        lock_guard<mutex> lock(aliveLock);
        if (!alive) throw runtime_error("Cannot schedule on destroyed ThreadPool");
    }

    {
        lock_guard<mutex> lock(queueLock);
        taskQueue.push(thunk);
    }

    {
        lock_guard<mutex> lock(pendingLock);
        pendingTasks++;
    }

    dispatcherReady.signal();
}

void ThreadPool::dispatcher() {
    while (!done) {
        dispatcherReady.wait();

        function<void(void)> task;
        {
            lock_guard<mutex> lock(queueLock);
            if (taskQueue.empty()) continue;
            task = taskQueue.front();
            taskQueue.pop();
        }

        for (;;) {
            for (size_t i = 0; i < wts.size(); ++i) {
                worker_t& worker = wts[i];
                bool expected = true;
                if (worker.available.compare_exchange_strong(expected, false)) {
                    {
                        lock_guard<mutex> lock(worker.mtx);
                        worker.thunk = task;
                    }
                    worker.ready.signal();
                    goto dispatch_next;
                }
            }
            this_thread::yield();
        }
    dispatch_next:
        continue;
    }
}

void ThreadPool::worker(int id) {
    worker_t& worker = wts[id];
    while (true) {
        worker.ready.wait();
        if (done) break;

        function<void(void)> taskCopy;
        {
            lock_guard<mutex> lock(worker.mtx);
            taskCopy = worker.thunk;
        }
        taskCopy();
        worker.available = true;

        {
            lock_guard<mutex> lock(pendingLock);
            pendingTasks--;
            if (pendingTasks == 0) {
                tasksDoneCV.notify_all();
            }
        }
    }
}

void ThreadPool::wait() {
    unique_lock<mutex> lock(pendingLock);
    tasksDoneCV.wait(lock, [this]() {
        return pendingTasks == 0;
    });
}

ThreadPool::~ThreadPool() {
    wait();
    done = true;

    dispatcherReady.signal();
    for (auto& worker : wts) worker.ready.signal();

    if (dt.joinable()) dt.join();
    for (auto& worker : wts) {
        if (worker.ts.joinable()) worker.ts.join();
    }

    {
        lock_guard<mutex> lock(aliveLock);
        alive = false;
    }
}
