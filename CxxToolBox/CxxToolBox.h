#pragma once

#include <chrono>
#include <future>
#include <functional>

namespace CxxToolBox
{
	class StopWatch
	{
	public:
		StopWatch() : start({}), end({}) 
		{

		}

		void Start()
		{
			start = std::chrono::steady_clock::now();
		}

		void Stop()
		{
			end = std::chrono::steady_clock::now();
		}

		void Reset()
		{
			start = end = { };
		}

		long long ElapsedSeconds()
		{
			return std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
		}

		long long ElapsedMilliSeconds()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		}

		long long ElapsedMicroSeconds()
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		}

		long long ElapsedNanoSeconds()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		}

	private:
		std::chrono::time_point<std::chrono::steady_clock> start;
		std::chrono::time_point<std::chrono::steady_clock> end;
	};

	template <class Return>
	class AsyncWorker
	{
	public:
		AsyncWorker() : function(nullptr), thread(nullptr)
		{

		}
		~AsyncWorker()
		{
			if (thread != nullptr)
			{
				if (thread->joinable())
					thread->join();
				delete thread;
			}
		}

		template<class Functor, class  ... Parameters>
		void Run(Functor && f, Parameters && ... parameters)
		{
			future = promise.get_future();
			function = std::bind(f, parameters...);
			thread = new std::thread(std::bind(&AsyncWorker::worker, this));
		}

		Return Result()
		{
			future.wait();
			thread->join();
			return future.get();
		}

	protected:
		std::promise<Return> promise;
		std::future<Return> future;
		std::function<Return()> function;
		std::thread * thread;

		void worker()
		{
			promise.set_value(function());
		}
	};

	template <>
	void AsyncWorker<void>::worker()
	{
		function();
		promise.set_value();
	}
}