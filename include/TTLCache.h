#pragma once
#include "BaseCache.h"
#include <chrono>
#include <thread>
#include <mutex>


template<class Tkey, class Tvalue>
class LazyTTLCache : public BaseCache<Tkey, Tvalue>{
private:
	using clock = std::chrono::steady_clock;
	size_t capacity;
	std::chrono::duration<double> life_time;
	std::unordered_map<Tkey, std::chrono::time_point<clock>> key_time;

	bool make_room() { // затычка
		bool isErased = false;
		auto it = key_time.begin();
		auto now = clock::now();
		while (it != key_time.end()) {
			if (now - it->second > life_time) {
				this->storage.erase(it->first);
				it = key_time.erase(it);
				isErased = true;
			}
			else it++;
		}
		return isErased;
	}

public:
	LazyTTLCache(size_t capacity, std::chrono::duration<double> life_time) : capacity(capacity), life_time(life_time) {
		if (capacity == 0) throw std::runtime_error("storage size must be greater than zero");
	}

	Tvalue& get(const Tkey& key) override {
		auto now = clock::now();
		auto _pair = this->storage.find(key);
		if (_pair == this->storage.end()) {
			throw std::runtime_error("the key was not found in the storage");
		}
		else if (now - key_time[key] > life_time) {
			this->storage.erase(key);
			this->key_time.erase(key);
			throw std::runtime_error("the key has expired");
		}
		return _pair->second;
	}

	void erase(const Tkey& key) override {
		bool isErased = this->storage.erase(key);
		if (isErased == 0) {
			throw std::runtime_error("the key was not found in the storage");
		}
		key_time.erase(key);
	}

	void put(const Tkey& key, const Tvalue& item) override {
		make_room();
		if (this->storage.size() >= capacity) {
			throw std::runtime_error("storage is full. Unable to clear space");
		}
		key_time[key] = clock::now();
		this->storage[key] = item;
	}

};

template<class Tkey, class Tvalue>
class ImmediateTTLCache : public BaseCache<Tkey, Tvalue> {
private:

	void immediate_make_room() { // затычка
		while (isRunning.load()) {
			{
				std::lock_guard<std::mutex> lock(this->mtx);
				auto it = key_time.begin();
				auto now = clock::now();
				while (it != key_time.end()) {
					if (now - it->second > life_time) {
						this->storage.erase(it->first);
						it = key_time.erase(it);

					}
					else it++;
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	using clock = std::chrono::steady_clock;
	size_t capacity;
	std::chrono::duration<double> life_time;
	std::unordered_map<Tkey, std::chrono::time_point<clock>> key_time;
	std::thread immediate_eraser;
	std::atomic<bool> isRunning{ true };

public:
	ImmediateTTLCache(size_t capacity, std::chrono::duration<double> life_time) : capacity(capacity), life_time(life_time) {
		if (capacity == 0) throw std::runtime_error("storage size must be greater than zero");
		immediate_eraser = std::thread(&ImmediateTTLCache::immediate_make_room, this);
	}
	~ImmediateTTLCache() {
		isRunning.store(false);
		if (immediate_eraser.joinable()) {
			immediate_eraser.join();
		}
	}

	Tvalue& get(const Tkey& key) override {
		std::lock_guard<std::mutex> lock(this->mtx);
		auto now = clock::now();
		auto _pair = this->storage.find(key);
		if (_pair == this->storage.end()) {
			throw std::runtime_error("the key was not found in the storage");
		}
		else if (now - key_time[key] > life_time) {
			this->storage.erase(key);
			this->key_time.erase(key);
			throw std::runtime_error("the key has expired");
		}
		return _pair->second;
	}

	void erase(const Tkey& key) override {
		std::lock_guard<std::mutex> lock(this->mtx);
		bool isErased = this->storage.erase(key);
		if (isErased == 0) {
			throw std::runtime_error("the key was not found in the storage");
		}
		key_time.erase(key);
	}

	void put(const Tkey& key, const Tvalue& item) override {
		std::lock_guard<std::mutex> lock(this->mtx);
		if (this->storage.size() >= capacity) {
			throw std::runtime_error("storage is full. Unable to clear space");
		}
		key_time[key] = clock::now();
		this->storage[key] = item;
	}

};