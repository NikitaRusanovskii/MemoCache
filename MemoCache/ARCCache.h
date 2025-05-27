#pragma once
#include <list>
#include <unordered_map>
#include <algorithm>
#include <deque>
#include <stdexcept>



template<class Tkey, class Tvalue>
class ARCCache {
private:
	std::list<Tkey> once_used_lru, reusable_lru;
	std::unordered_map<Tkey, std::pair<Tvalue, std::list<Tkey>::iterator>> once_used, reusable;
	std::deque<Tkey> removed_from_once_used, removed_from_reusable;

	size_t capacity;
	int list_pointer;

	void replace() {
		
	}

public:

	ARCCache(size_t capacity) : capacity(capacity) {
		if (capacity == 0) throw std::runtime_error("storage size must be greater than zero");
		list_pointer = 0;
	}

	size_t size() {
		return this->capacity;
	}

	void put(Tkey& key, Tvalue& value) {
		if (once_used.find(key) != once_used.end()) {
			once_used_lru.erase(once_used[key].second);
			once_used.erase(key);
			reusable_lru.push_front(key);
			reusable[key] = std::pair{ value, reusable_lru.begin() };
			return;
		}
		if (reusable.find(key) != reusable.end()) {
			reusable_lru.erase(reusable[key].second);
			reusable_lru.push_front(key);
			reusable[key] = std::pair{ value, reusable_lru.begin() };
			return;
		}

		auto it = std::find(removed_from_once_used.begin(), removed_from_once_used.end(), key);
		if (it != removed_from_once_used.end()) {
			list_pointer += 1;

			// replace();

			removed_from_once_used.erase(it);
			reusable_lru.push_front(key);
			reusable[key] = std::pair{ value, reusable_lru.begin() };
			return;
		}
		auto it2 = std::find(removed_from_reusable.begin(), removed_from_reusable.end(), key);
		if (it2 != removed_from_reusable.end()) {
			list_pointer -= 1;

			// replace();

			removed_from_reusable.erase(it2);
			reusable_lru.push_front(key);
			reusable[key] = std::pair{ value, reusable_lru.begin() };
			return;
		}


		if (reusable.size() + once_used.size() == capacity) {
			// replace();
		}
		once_used_lru.push_front(key);
		once_used[key] = std::pair{ value, once_used_lru.begin() };
	}

};