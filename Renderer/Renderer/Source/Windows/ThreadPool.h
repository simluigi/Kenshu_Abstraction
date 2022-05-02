#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <future>
#include <condition_variable>
#include <memory>

class ThreadPool
{
public:

	~ThreadPool();

	static void CreateThread();

	static std::future<void> AddTask(std::function<void()> func);

private:

	ThreadPool();
	ThreadPool(const ThreadPool&) = delete;
	void operator=(const ThreadPool&) = delete;

	static ThreadPool& Instance();

	void WorkerThread();

	struct Task
	{
		std::function<void()> func;
		std::promise<void> promise;
	};

	std::mutex m_mutex;
	std::vector<std::thread> m_workerThreads;
	std::condition_variable m_conditionVariable;
	std::queue<Task> m_taskQueue;

	bool m_terminate = false;
};

