#pragma once

#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <future>
#include <functional>

class ThreadPool
{
private:
	std::vector<std::jthread> workers;
	std::queue<std::move_only_function<void()>> queue;
	std::mutex queue_mutex;
	std::condition_variable_any condvar;

public:
	ThreadPool(size_t nt = std::thread::hardware_concurrency())
	{
		if (nt == 0) [[unlikely]]
			throw std::invalid_argument("Need at least 1 worker thread!");

		while (nt--)
		{
			workers.emplace_back(
				[this](std::stop_token st)
				{
					while (true)
					{
						std::unique_lock<std::mutex> lock(queue_mutex);
						condvar.wait(lock, st, [this] { return !queue.empty(); });

						if (st.stop_requested()) // lock releases on destruction
							break;

						auto fcn = std::move(queue.front());
						queue.pop();
						lock.unlock();

						fcn();
					}
				}
			);
		}
	}

	~ThreadPool()
	{
		for (auto& thread : workers)
			thread.request_stop();

		condvar.notify_all();
		for (auto& thread : workers)
			thread.join();
	}

	template<typename F, typename... Args>
	auto push(F&& f, Args&&... args)
	{
		using R = std::invoke_result_t<F, Args...>;

		std::packaged_task<R(Args...)> task(std::forward<F>(f));

		std::future<R> future = task.get_future();

		{
			std::lock_guard<std::mutex> lock(queue_mutex);
			queue.emplace(
				[task = std::move(task), ... args = std::forward<Args>(args)]() mutable
				{
					task(args...);
				}
			);
		}

		condvar.notify_one();
		return future;
	}
};
