#pragma once
#include <shared_mutex>
#include <list>
#include <vector>
#include <map>
#include <concepts>




template<typename Key, typename Value, typename Hash = std::hash<Key>>
class MutexMap {
private:

	class bucket
	{
	private:
		typedef std::pair<Key, Value> node;
		typedef std::list<node> dataV;
		dataV data;

		typedef typename dataV::iterator bucket_iterator;
		mutable std::shared_mutex mutex;

		bucket_iterator find(const Key& key) {
			bucket_iterator iter = data.begin();
			while (iter != data.end() and iter->first != key) {
				iter++;
			}
			return iter;
		}
		bucket_iterator find(Key&& key) {
			bucket_iterator iter = data.begin();
			while (iter != data.end() and iter->first != key) {
				iter++;
			}
			return iter;
		}

		void add(Key&& key, Value&& value) {
			data.push_back({ (key), (value) });
		}

		void add(const Key& key, const Value& value) {
			data.push_back({ (key), (value) });
		}

	public:

		bucket() {

		}

		void insert(Key&& key, Value&& value) {
			std::unique_lock<std::shared_mutex> write_lock(mutex);
			bucket_iterator iter = find((key));
			if (iter != data.end())return;
			add((key), (value));
		}
		void insert(const Key& key, const Value& value) {
			std::unique_lock<std::shared_mutex> write_lock(mutex);
			bucket_iterator iter = find((key));
			if (iter != data.end())return;
			add((key), (value));
		}

		 Value& get(Key&& key) {
			{
				std::shared_lock<std::shared_mutex> lock(mutex);
				bucket_iterator iter = find((key));
				if (iter != data.end()) return iter->second;
			}

			std::unique_lock<std::shared_mutex> lock(mutex);
			bucket_iterator iter = find((key));
			if (iter != data.end()) {
				return iter->second;
			}
			data.emplace_back(key, Value{});
			return data.back().second;
		}
		Value& get(const Key& key) {
			{
				std::shared_lock<std::shared_mutex> lock(mutex);
				bucket_iterator iter = find((key));
				if (iter != data.end()) return iter->second;
			}

			std::unique_lock<std::shared_mutex> lock(mutex);
			bucket_iterator iter = find((key));
			if (iter != data.end()) {
				return iter->second;
			}
			data.emplace_back(key, Value{});
			return data.back().second;
		}

		void change(Key&& key, Value&& value) {
			std::unique_lock<std::shared_mutex> write_lock(mutex);
			bucket_iterator iter = find((key));

			if (iter != data.end()) {
				iter->second = value;
			}
			else {
				add((key), (value));
			}
		}
		void change(const Key& key, const Value& value) {
			std::unique_lock<std::shared_mutex> write_lock(mutex);
			bucket_iterator iter = find((key));

			if (iter != data.end()) {
				iter->second = value;
			}
			else {
				add((key), (value));
			}
		}

		void erase(Key&& key) {
			std::unique_lock<std::shared_mutex> write_lock(mutex);
			bucket_iterator iter = find((key));
			if (iter != data.end()) {
				data.erase(iter);
			}

		}
		void erase(const Key& key) {
			std::unique_lock<std::shared_mutex> write_lock(mutex);
			bucket_iterator iter = find((key));
			if (iter != data.end()) {
				data.erase(iter);
			}

		}

		bool contains(Key&& key) {
			std::shared_lock<std::shared_mutex> lock(mutex);
			return (find(key) != data.end());
		}
		bool contains(const Key& key) {
			std::shared_lock<std::shared_mutex> lock(mutex);
			return (find(key) != data.end());
		}
		template<typename MapType>
		void output(MapType& in) {
			std::shared_lock<std::shared_mutex> lock(mutex);
			for (auto& el : data) {
				in.insert(el);
			}
		}
	};

	size_t num_buckets;

	Hash hash_func;

	std::vector<std::unique_ptr<bucket>> buckets;

	bucket& get_bucket(Key&& key) {
		return *buckets[(hash_func((key))) % num_buckets];
	}
	bucket& get_bucket(const Key& key) {
		return *buckets[(hash_func((key))) % num_buckets];
	}


public:


	MutexMap(size_t num_buckets_,
		Hash const& hash_func_ = Hash()) : num_buckets(num_buckets_), hash_func(hash_func_), buckets(num_buckets_) {

		for (size_t i = 0; i < num_buckets; ++i) {
			buckets[i] = (std::make_unique<bucket>());
		}
	}
	MutexMap(const MutexMap& cpy) = delete;
	MutexMap& operator=(const MutexMap& cpy) = delete;

	Value& get(Key&& key) {
		return get_bucket((key)).get((key));
	}
	Value& get(const Key& key) {
		return get_bucket((key)).get((key));
	}

	void insert(Key&& key, Value&& value) {
		get_bucket((key)).insert((key), (value));
	}
	void insert(const Key& key, const Value& value) {
		get_bucket((key)).insert((key), (value));
	}

	void erase(Key&& key) {
		get_bucket((key)).erase((key));
	}
	void erase(const Key& key) {
		get_bucket((key)).erase((key));
	}

	void change(Key&& key, Value&& value) {
		get_bucket((key)).change((key), (value));
	}
	void change(const Key& key, const Value& value) {
		get_bucket((key)).change((key), (value));
	}

	Value& operator[](Key&& key) {
		return get((key));
	}
	Value& operator[](const Key& key) {
		return get((key));
	}


	bool contains(Key&& key) {
		return get_bucket((key)).contains((key));
	}
	bool contains(const Key& key) {
		return get_bucket((key)).contains((key));
	}

	template<typename MapType>
	MapType getMap() {
		MapType ans;
		for (size_t i = 0; i < num_buckets; ++i) {
			(*buckets[i]).output(ans);
		}
		return ans;
	}
};