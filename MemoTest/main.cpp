#include <iostream>
#include <thread>
#include <locale>
#include "LRUCache.h"
#include "LFUCache.h"
#include "TTLCache.h"


void testImmediateTTL() {
    std::cout << "\n=== ������������ ImmediateTTLCache ===" << std::endl;

    ImmediateTTLCache<int, std::string> cache(3, std::chrono::seconds(2));

    std::cout << "��������� ����� 1 � 2..." << std::endl;
    cache.put(1, "����");
    cache.put(2, "���");

    std::cout << "�������� ���� 1: ";
    try {
        std::cout << cache.get(1) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "������: " << e.what() << std::endl;
    }

    std::cout << "��� 3 ������� ��� ��������� TTL..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "������� �������� ���������� ���� 1: ";
    try {
        std::cout << cache.get(1) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "��������� ����������: " << e.what() << std::endl;
    }

    std::cout << "��������� ����� ���� 3..." << std::endl;
    try {
        cache.put(3, "���");
        std::cout << "������� �������� 3: " << cache.get(3) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "������ ��� ���������� ��� ���������: " << e.what() << std::endl;
    }

    std::cout << "������� ���� 3..." << std::endl;
    try {
        cache.erase(3);
        std::cout << "���� 3 �����." << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "������ ��� ��������: " << e.what() << std::endl;
    }

    std::cout << "������� �������� �������� ���� 3: ";
    try {
        std::cout << cache.get(3) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "��������� ���������� ����� ��������: " << e.what() << std::endl;
    }
}

void testLazyTTL() {
    std::cout << "\n=== ������������ LazyTTLCache ===" << std::endl;

    LazyTTLCache<int, std::string> ttlCache(3, std::chrono::seconds(2));

    std::cout << "��������� ����� 1 � 2..." << std::endl;
    ttlCache.put(1, "����");
    ttlCache.put(2, "���");

    std::cout << "�������� ���� 1: " << ttlCache.get(1) << std::endl;

    std::cout << "��� 3 ������� ��� ��������� TTL..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));

    try {
        std::cout << "������� �������� ���������� ���� 1..." << std::endl;
        std::cout << "�������� 1: " << ttlCache.get(1) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "��������� ����������: " << e.what() << std::endl;
    }

    try {
        std::cout << "��������� ����� ���� 3..." << std::endl;
        ttlCache.put(3, "���");
        std::cout << "������� �������� 3: " << ttlCache.get(3) << std::endl;
    }
    catch (const std::runtime_error& e) {
        std::cout << "����������� ������ ��� ����������: " << e.what() << std::endl;
    }

    std::cout << "������� ���� 3..." << std::endl;
    ttlCache.erase(3);

    try {
        std::cout << "������� �������� �������� ���� 3..." << std::endl;
        ttlCache.get(3);
    }
    catch (const std::runtime_error& e) {
        std::cout << "��������� ���������� ����� ��������: " << e.what() << std::endl;
    }
}


void testLRU() {
    std::cout << "\n=== ������������ LRUCache ===" << std::endl;
    LRUCache<int, std::string> lru(3);

    lru.put(1, "����");
    lru.put(2, "���");
    lru.put(3, "���");

    std::cout << "�������� 1: " << lru.get(1) << std::endl;

    lru.put(4, "������"); // ��� ������ ��������� ���� 2 (�������� ������� ��������������)

    try {
        std::cout << "�������� �������� ���������� ���� 2..." << std::endl;
        std::cout << "�������� 2: " << lru.get(2) << std::endl;
    }
    catch (std::runtime_error& e) {
        std::cout << "��������� ����������: " << e.what() << std::endl;
    }

    lru.erase(3);
    try {
        lru.get(3);
    }
    catch (std::runtime_error& e) {
        std::cout << "��������� ���������� ����� ��������: " << e.what() << std::endl;
    }
}

void testLFU() {
    std::cout << "\n=== ������������ LFUCache ===" << std::endl;
    LFUCache<int, std::string> lfu(3);

    lfu.put(1, "����");
    lfu.put(2, "���");
    lfu.put(3, "���");

    // ����������� ������� ������������� ����� 1
    lfu.get(1);
    lfu.get(1);

    // ����������� ������� ������������� ����� 2
    lfu.get(2);

    lfu.put(4, "������"); // ������ ���� �������� ���� 3 (�������� ����� ������������)

    try {
        std::cout << "�������� �������� ���������� ���� 3..." << std::endl;
        std::cout << "�������� 3: " << lfu.get(3) << std::endl;
    }
    catch (std::runtime_error& e) {
        std::cout << "��������� ����������: " << e.what() << std::endl;
    }

    lfu.erase(1);
    try {
        lfu.get(1);
    }
    catch (std::runtime_error& e) {
        std::cout << "��������� ���������� ����� ��������: " << e.what() << std::endl;
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
