#pragma once

#include <chrono>

namespace CxxToolBox
{
	class StopWatch
	{
	public:
		StopWatch() : start({}), end({}) {}

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
}