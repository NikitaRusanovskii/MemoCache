#pragma once
#include "BaseCache.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>


template<class Tkey, class Tvalue>
class LazyTTLCache : public BaseCache<Tkey, Tvalue> {
private:
	using clock = std::chrono::steady_clock;
	size_t capacity;
	std::chrono::duration<double> life_time;
	std::unordered_map<Tkey, std::chrono::time_point<clock>> key_time;
	std::mutex mtx;

	bool make_room() {
		bool erased = false;
		auto now = clock::now();
		for (auto it = key_time.begin(); it != key_time.end(); ) {
			if (now - it->second > life_time) {
				this->storage.erase(it->first);
				it = key_time.erase(it);
				erased = true;
			} else {
				++it;
			}
		}
		return erased;
	}

public:
	LazyTTLCache(size_t capacity, std::chrono::duration<double> life_time)
		: capacity(capacity), life_time(life_time) {
		if (capacity == 0)
			throw std::runtime_error("storage size must be greater than zero");
	}

	Tvalue& get(const Tkey& key) override {
		std::lock_guard<std::mutex> lock(mtx);
		auto now = clock::now();
		auto it = this->storage.find(key);
		if (it == this->storage.end())
			throw std::runtime_error("the key was not found in the storage");

		if (now - key_time[key] > life_time) {
			this->storage.erase(key);
			key_time.erase(key);
			throw std::runtime_error("the key has expired");
		}
		return it->second;
	}

	void put(const Tkey& key, const Tvalue& item) override {
		std::lock_guard<std::mutex> lock(mtx);
		make_room();
		if (this->storage.size() >= capacity)
			throw std::runtime_error("storage is full. Unable to clear space");

		key_time[key] = clock::now();
		this->storage[key] = item;
	}

	bool contains(const Tkey& key) override {
		std::lock_guard<std::mutex> lock(mtx);
		return this->storage.find(key) != this->storage.end();
	}

	void erase(const Tkey& key) override {
		std::lock_guard<std::mutex> lock(mtx);
		bool erased = this->storage.erase(key);
		key_time.erase(key);
		if (!erased)
			throw std::runtime_error("the key was not found in the storage");
	}

	void clear() override {
		std::lock_guard<std::mutex> lock(mtx);
		this->storage.clear();
		key_time.clear();
	}

	size_t size() const {
		std::lock_guard<std::mutex> lock(mtx);
		return this->storage.size();
	}

};


template<class Tkey, class Tvalue>
class ImmediateTTLCache : public BaseCache<Tkey, Tvalue> {
private:
	using clock = std::chrono::steady_clock;
	size_t capacity;
	std::chrono::duration<double> life_time;
	std::unordered_map<Tkey, std::chrono::time_point<clock>> key_time;
	std::thread cleaner;
	std::mutex mtx;
	std::atomic<bool> isRunning{true};

	void background_cleaner() {
		while (isRunning.load()) {
			{
				std::lock_guard<std::mutex> lock(mtx);
				auto now = clock::now();
				for (auto it = key_time.begin(); it != key_time.end(); ) {
					if (now - it->second > life_time) {
						this->storage.erase(it->first);
						it = key_time.erase(it);
					} else {
						++it;
					}
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

public:
	ImmediateTTLCache(size_t capacity, std::chrono::duration<double> life_time)
		: capacity(capacity), life_time(life_time) {
		if (capacity == 0)
			throw std::runtime_error("storage size must be greater than zero");
		cleaner = std::thread(&ImmediateTTLCache::background_cleaner, this);
	}

	~ImmediateTTLCache() {
		isRunning.store(false);
		if (cleaner.joinable())
			cleaner.join();
	}

	Tvalue& get(const Tkey& key) override {
		std::lock_guard<std::mutex> lock(mtx);
		auto now = clock::now();
		auto it = this->storage.find(key);
		if (it == this->storage.end())
			throw std::runtime_error("the key was not found in the storage");

		if (now - key_time[key] > life_time) {
			this->storage.erase(key);
			key_time.erase(key);
			throw std::runtime_error("the key has expired");
		}
		return it->second;
	}

	void put(const Tkey& key, const Tvalue& item) override {
		std::lock_guard<std::mutex> lock(mtx);
		if (this->storage.size() >= capacity)
			throw std::runtime_error("storage is full. Unable to clear space");

		key_time[key] = clock::now();
		this->storage[key] = item;
	}

	void erase(const Tkey& key) override {
		std::lock_guard<std::mutex> lock(mtx);
		bool erased = this->storage.erase(key);
		key_time.erase(key);
		if (!erased)
			throw std::runtime_error("the key was not found in the storage");
	}

	bool contains(const Tkey& key) override {
		std::lock_guard<std::mutex> lock(mtx);
		return this->storage.find(key) != this->storage.end();
	}

	void clear() override {
		std::lock_guard<std::mutex> lock(mtx);
		this->storage.clear();
		key_time.clear();
	}

	size_t size() const {
		std::lock_guard<std::mutex> lock(mtx);
		return this->storage.size();
	}

};
