#include "ThreadPool.h"

constexpr unsigned int WORKER_THREAD_MAX_COUNT = 4;

ThreadPool::ThreadPool()
{
	for (unsigned int threadCount = 0; threadCount < std::thread::hardware_concurrency(); ++threadCount)
	{
		m_workerThreads.emplace_back(std::thread([this] { WorkerThread(); }));
	}
}

ThreadPool& ThreadPool::Instance()
{
	static ThreadPool instance;
	return instance;
}

void ThreadPool::WorkerThread()
{
	while (true)
	{
		Task task = {};

		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_conditionVariable.wait(lock, [&] {return m_terminate || !m_taskQueue.empty(); });

			if (m_terminate && m_taskQueue.empty())
			{
				break;
			}

			if (m_taskQueue.empty())
			{
				continue;
			}

			task = std::move(m_taskQueue.front());
			m_taskQueue.pop();
		}

		task.func();
		task.promise.set_value();
		m_conditionVariable.notify_one();
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_terminate = true;
	}

	m_conditionVariable.notify_all();

	for (auto& thread : m_workerThreads)
	{
		thread.join();
	}
}

void ThreadPool::CreateThread()
{
	Instance();
}

std::future<void> ThreadPool::AddTask(std::function<void()> func)
{
	auto& renderThreadPool = Instance();

	std::lock_guard<std::mutex> lock(renderThreadPool.m_mutex);

	renderThreadPool.m_taskQueue.push(Task(func));
	renderThreadPool.m_conditionVariable.notify_one();
	return renderThreadPool.m_taskQueue.back().promise.get_future();
}
