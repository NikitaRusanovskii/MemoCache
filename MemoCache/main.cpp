#include "LRUCache.h"
#include <iostream>
#include <exception>


using namespace std;


int main() {
    try {
        LRUCache<int, int> my_lru(0); // пробуем создать хранилище нулевого размера
    }
    catch (exception ex) {
        cout << "TEST 1 - zero size storage - " << ex.what() << endl;
    }

    LRUCache<int, int> my_lru(10); // создаём нормальное хранилище

    try {
        my_lru.get(1344); // пробуем получить данные по несуществующему ключу
    }
    catch (exception ex) {
        cout << "TEST 2 - non-existent key - " << ex.what() << endl;
    }

    try {
        my_lru.erase(4124); // пробуем удалить данные по несуществующему ключу
    }
    catch (exception ex) {
        cout << "TEST 3 - deleting data by non-existent key - " << ex.what() << endl;
    }

    my_lru.put(3, 3);
    my_lru.put(4, 5);
    my_lru.put(3, 4);

    cout << "VALUE 1: " << my_lru.get(3) << " ||| VALUE 2: " << my_lru.get(4) << endl;
    my_lru.erase(3);
    
    try {
        my_lru.get(3);
    }
    catch (exception ex) {
        cout << "TEST 4 - non-existent key again - " << ex.what() << endl;
    }

    return 0;

}