#pragma once
#include "BaseCache.h"
#include <unordered_map>
#include <list>
#include <stdexcept>


template<class Tkey, class Tvalue>
class LRUCache : public BaseCache<Tkey, Tvalue> {
private:
	size_t max_num_of_elements;
	/*
		Here I learned that the compiler did not understand that std::list<Tkey>::iterator is a type.
			Therefore, I had to use the new keyword typename, which explicitly tells the compiler
			that the type name comes after it.
		Здесь узнал, что компилятор не понял, что std::list<Tkey>::iterator - есть тип.
			Поэтому пришлось использовать новое ключевое слово typename, которое явно говорит компилятору,
			что после него идёт имя типа.
	*/
	std::unordered_map<Tkey, typename std::list<Tkey>::iterator> key_iter;
	std::list<Tkey> lru_list;
public:
	LRUCache(size_t max_num_of_elements) : max_num_of_elements(max_num_of_elements) {
		if (max_num_of_elements == 0) throw std::runtime_error("storage size must be greater than zero");
	};

	void put(const Tkey& key, const Tvalue& item) override {
		auto _pair = this->storage.find(key);
		if (_pair != this->storage.end()) {
			lru_list.erase(key_iter[key]);
		}
		else {
			if (max_num_of_elements <= lru_list.size()) {
				auto oldKey = lru_list.back();
				lru_list.pop_back();
				key_iter.erase(oldKey);
				this->storage.erase(oldKey);
			}
		}
		lru_list.push_front(key);
		key_iter[key] = lru_list.begin();
		this->storage[key] = item;
	}

	Tvalue& get(const Tkey& key) override {
		auto iter_pair = this->storage.find(key); // вернул итератор
		if (iter_pair == this->storage.end()) {
			throw std::runtime_error("the key was not found in the storage");
		}
		lru_list.erase(key_iter[key]);
		lru_list.push_front(key);
		key_iter[key] = lru_list.begin();
		return iter_pair->second; // разыменовал итератор(ссылка на пару)
	}

	void erase(const Tkey& key) override {
		bool isErased = this->storage.erase(key);
		if (isErased == 0) {
			throw std::runtime_error("the key was not found in the storage");
		}
		lru_list.erase(key_iter[key]);
		key_iter.erase(key);
	}
};


/*
	Some characteristics:
		On average, the speed of put, get, erase operations is O(1)
	Немного характеристик:
		В среднем, скорость операций put, get, erase составляет O(1)
*/
