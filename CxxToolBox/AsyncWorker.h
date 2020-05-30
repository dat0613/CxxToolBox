#pragma once

#include <future>
#include <functional>

namespace CxxToolBox
{
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