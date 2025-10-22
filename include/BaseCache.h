#pragma once
#include <unordered_map>
#include <stdexcept>
#include <list>

template<class Tkey, class Tvalue>
class BaseCache {
protected:
	std::unordered_map<Tkey, Tvalue> storage;
public:

	virtual Tvalue& get(const Tkey& key) {
		auto iter_pair = storage.find(key); // вернул итератор
		if (iter_pair == storage.end()) {
			throw std::runtime_error("the key was not found in the storage");
		}
		return iter_pair->second; // разыменовал итератор(ссылка на пару)
	}
	virtual void put(const Tkey& key, const Tvalue& item) {
		storage[key] = item;
	}
	virtual bool contains(const Tkey& key) {
		if (storage.find(key) == storage.end()) {
			return false;
		}
		return true;
	}
	virtual void erase(const Tkey& key) {
		bool isErased = storage.erase(key);
		if (isErased == 0) {
			throw std::runtime_error("the key was not found in the storage");
		}
	}
	virtual void clear() {
		storage.clear();
	}
	virtual size_t size() const {
		return this->storage.size();
	}
};