#pragma once
#include "BaseCache.h"
#include <list>
#include <algorithm>

template<class Tkey, class Tvalue>
class LFUCache : public BaseCache<Tkey, Tvalue> {
private:

    struct KaC { // key and using count
        Tkey key;
        size_t using_count;
    };

    std::list<KaC> lfu_list; // storage pair (key, number of accesses)
    std::unordered_map<Tkey, typename std::list<KaC>::iterator> key_iter; // access iterator by key
    size_t capacity;
public:
    LFUCache(size_t capacity) : capacity(capacity) {
        if (capacity == 0) throw std::runtime_error("storage size must be greater than zero");
    };

    void put(const Tkey& key, const Tvalue& item) override {
        if (this->storage.size() >= this->capacity) {
            auto key_to_delete = lfu_list.back().key;
            this->storage.erase(key_to_delete);
            key_iter.erase(key_to_delete);
            lfu_list.pop_back();
        }

        auto _pair = this->storage.find(key);
        if (_pair == this->storage.end()) {
            lfu_list.emplace_back(KaC{ key, 1 });
            if (lfu_list.size() > 1) {
                key_iter[key] = std::prev(lfu_list.end());
            }
            else {
                key_iter[key] = lfu_list.begin();
            }
        }
        else {
            key_iter[key]->using_count++;
            if (lfu_list.size() > 1) {
                auto it1 = key_iter[key];
                if (it1 != lfu_list.begin()) {
                    auto it2 = std::prev(key_iter[key]);
                    while (it1->using_count > it2->using_count) {
                        std::iter_swap(it1, it2);
                        key_iter[it1->key] = it2;
                        key_iter[it2->key] = it1;
                        if (it1 == lfu_list.begin()) break;
                    }
                }
            }
        }

        this->storage[key] = item;
    }

    Tvalue& get(const Tkey& key) override {
        auto iter_pair = this->storage.find(key);
        if (iter_pair == this->storage.end()) {
            throw std::runtime_error("the key was not found in the storage");
        }
        key_iter[key]->using_count++;
        auto it1 = key_iter[key];
        if (it1 != lfu_list.begin()) {
            auto it2 = std::prev(key_iter[key]);
            while (it1->using_count > it2->using_count) {
                std::iter_swap(it1, it2);
                key_iter[it1->key] = it2;
                key_iter[it2->key] = it1;
                if (it1 == lfu_list.begin()) break;
            }
        }

        return iter_pair->second;
    }

    void erase(const Tkey& key) override {
        bool isErased = this->storage.erase(key);
        if (isErased == 0) {
            throw std::runtime_error("the key was not found in the storage");
        }
        lfu_list.erase(key_iter[key]);
        key_iter.erase(key);
    }

};

/* далее я собираюсь заменить структуру KaC, list<KaC> и unordered_map<Tkey, std::list<KaC>::iterator> на map<int, list<Tkey>>
для улучшения производительности*/