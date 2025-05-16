#include <iostream>
#include <thread>
#include <locale>
#include "LRUCache.h"
#include "LFUCache.h"
#include "TTLCache.h"


void testImmediateTTL() {
    std::cout << "\n=== Тестирование ImmediateTTLCache ===" << std::endl;

    ImmediateTTLCache<int, std::string> cache(3, std::chrono::seconds(2));

    std::cout << "Добавляем ключи 1 и 2..." << std::endl;
    cache.put(1, "один");
    cache.put(2, "два");

    std::cout << "Получаем ключ 1: ";
    try {
        std::cout << cache.get(1) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Ошибка: " << e.what() << std::endl;
    }

    std::cout << "Ждём 3 секунды для истечения TTL..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "Пробуем получить устаревший ключ 1: ";
    try {
        std::cout << cache.get(1) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Ожидаемое исключение: " << e.what() << std::endl;
    }

    std::cout << "Добавляем новый ключ 3..." << std::endl;
    try {
        cache.put(3, "три");
        std::cout << "Успешно добавлен 3: " << cache.get(3) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Ошибка при добавлении или получении: " << e.what() << std::endl;
    }

    std::cout << "Удаляем ключ 3..." << std::endl;
    try {
        cache.erase(3);
        std::cout << "Ключ 3 удалён." << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Ошибка при удалении: " << e.what() << std::endl;
    }

    std::cout << "Пробуем получить удалённый ключ 3: ";
    try {
        std::cout << cache.get(3) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Ожидаемое исключение после удаления: " << e.what() << std::endl;
    }
}

void testLazyTTL() {
    std::cout << "\n=== Тестирование LazyTTLCache ===" << std::endl;

    LazyTTLCache<int, std::string> ttlCache(3, std::chrono::seconds(2));

    std::cout << "Добавляем ключи 1 и 2..." << std::endl;
    ttlCache.put(1, "один");
    ttlCache.put(2, "два");

    std::cout << "Получаем ключ 1: " << ttlCache.get(1) << std::endl;

    std::cout << "Ждём 3 секунды для истечения TTL..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    try {
        std::cout << "Пробуем получить устаревший ключ 1..." << std::endl;
        std::cout << "Получить 1: " << ttlCache.get(1) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Ожидаемое исключение: " << e.what() << std::endl;
    }

    try {
        std::cout << "Добавляем новый ключ 3..." << std::endl;
        ttlCache.put(3, "три");
        std::cout << "Успешно добавлен 3: " << ttlCache.get(3) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "Неожиданная ошибка при добавлении: " << e.what() << std::endl;
    }

    std::cout << "Удаляем ключ 3..." << std::endl;
    ttlCache.erase(3);

    try {
        std::cout << "Пробуем получить удалённый ключ 3..." << std::endl;
        ttlCache.get(3);
    }
    catch (const std::runtime_error& e) {
        std::cout << "Ожидаемое исключение после удаления: " << e.what() << std::endl;
    }
}


void testLRU() {
    std::cout << "\n=== Тестирование LRUCache ===" << std::endl;
    LRUCache<int, std::string> lru(3);

    lru.put(1, "один");
    lru.put(2, "два");
    lru.put(3, "три");

    std::cout << "Получить 1: " << lru.get(1) << std::endl;

    lru.put(4, "четыре"); // Это должно вытеснить ключ 2 (наименее недавно использованный)

    try {
        std::cout << "Пытаемся получить вытеснённый ключ 2..." << std::endl;
        std::cout << "Получить 2: " << lru.get(2) << std::endl;
    }
    catch (std::runtime_error& e) {
        std::cout << "Ожидаемое исключение: " << e.what() << std::endl;
    }

    lru.erase(3);
    try {
        lru.get(3);
    }
    catch (std::runtime_error& e) {
        std::cout << "Ожидаемое исключение после удаления: " << e.what() << std::endl;
    }
}

void testLFU() {
    std::cout << "\n=== Тестирование LFUCache ===" << std::endl;
    LFUCache<int, std::string> lfu(3);

    lfu.put(1, "один");
    lfu.put(2, "два");
    lfu.put(3, "три");

    // Увеличиваем частоту использования ключа 1
    lfu.get(1);
    lfu.get(1);

    // Увеличиваем частоту использования ключа 2
    lfu.get(2);

    lfu.put(4, "четыре"); // Должен быть вытеснен ключ 3 (наименее часто используемый)

    try {
        std::cout << "Пытаемся получить вытеснённый ключ 3..." << std::endl;
        std::cout << "Получить 3: " << lfu.get(3) << std::endl;
    }
    catch (std::runtime_error& e) {
        std::cout << "Ожидаемое исключение: " << e.what() << std::endl;
    }

    lfu.erase(1);
    try {
        lfu.get(1);
    }
    catch (std::runtime_error& e) {
        std::cout << "Ожидаемое исключение после удаления: " << e.what() << std::endl;
    }
}

int main() {

    setlocale(LC_ALL, "rus");

    testImmediateTTL();
    testLazyTTL();
    testLRU();
    testLFU();
    return 0;
}
