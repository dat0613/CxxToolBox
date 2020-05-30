#pragma once

#include <vector>
#include <shared_mutex>
#include <unordered_map>
#include <functional>

namespace CxxToolBox
{
	template<class K, class V>
	class concurrent_distributed_unordered_map
	{
	public:
		concurrent_distributed_unordered_map(size_t partition) : partition(partition), count(0), mutexes(partition), maps(partition)
		{
			for (int i = 0; i < partition; i++)
			{
				mutexes[i] = new std::shared_mutex();
				maps[i] = new std::unordered_map<K, V>();
			}
		}

		virtual ~concurrent_distributed_unordered_map()
		{
			for (int i = 0; i < partition; i++)
			{
				delete mutexes[i];
				delete maps[i];
			}
		}

		bool Insert(const K & key, V & value)
		{
			int hash = GetHashNumber(key);
			std::lock_guard<std::shared_mutex> m{ *mutexes[hash] };

			auto & map = (*maps[hash]);

			auto pair = map.find(key);
			if (pair == map.end())
			{
				count.fetch_add(1);
				map.insert(std::make_pair(key, value));
				return true;
			}
			return false;
		}

		bool Erase(const K & key)
		{
			int hash = GetHashNumber(key);
			std::lock_guard<std::shared_mutex> m{ *mutexes[hash] };

			auto & map = (*maps[hash]);

			auto pair = map.find(key);
			if (pair != map.end())
			{
				count.fetch_sub(1);
				map.erase(key);
				return true;
			}
			return false;
		}

		bool Empty() const
		{
			return count == 0;
		}

		void Clear()
		{
			if (Empty())
				return;

			for (int i = 0; i < partition; i++)
			{
				std::lock_guard<std::shared_mutex> m{ *mutexes[i] };

				auto & map = (*maps[i]);
				map.clear();
			}
		}

		bool Find(const K & key, V & out_value) const
		{
			int hash = GetHashNumber(key);
			std::shared_lock<std::shared_mutex> m{ *mutexes[hash] };

			auto & map = (*maps[hash]);

			auto pair = map.find(key);
			if (pair != map.end())
			{
				out_value = pair->second;
				return true;
			}
			return false;
		}

		void ForEach(std::function<void(V&)> f) const
		{
			for (int i = 0; i < partition; i++)
			{
				std::shared_lock<std::shared_mutex> m{ *mutexes[i] };

				auto & map = (*maps[i]);

				for (auto pair : map)
				{
					f(pair.second);
				}
			}
		}

		void Where(std::function<bool(const V&)> f, std::list<V> & out_result) const
		{
			for (int i = 0; i < partition; i++)
			{
				std::shared_lock<std::shared_mutex> m{ *mutexes[i] };

				auto & map = (*maps[i]);

				for (auto pair : map)
				{
					if (f(pair.second))
						out_result.push_back(pair.second);
				}
			}
		}

		int Partition() const
		{
			return partition;
		}

		int Count() const
		{
			return count;
		}

	protected:

		int GetHashNumber(const K key) const
		{
			return std::hash<K> {} (key) % partition;
		}

		size_t partition;
		std::atomic<int> count;
		std::vector<std::shared_mutex *> mutexes;
		std::vector<std::unordered_map<K, V> *> maps;
	};
}