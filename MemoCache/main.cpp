#include <iostream>
#include "LFUCache.h"

int main() {
    LFUCache<int, std::string> cache(3);  // Ємкость = 3

    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    std::cout << "Get 1: " << cache.get(1) << "\n"; // повышает частоту 1
    std::cout << "Get 1: " << cache.get(1) << "\n"; // ещЄ раз Ч частота 1 = 3
    std::cout << "Get 2: " << cache.get(2) << "\n"; // частота 2 = 2

    cache.put(4, "four"); // вытеснит 3, так как у него наименьша€ частота (1)

    try {
        cache.get(3); // должен кинуть исключение
    }
    catch (const std::exception& ex) {
        std::cout << "Exception caught: " << ex.what() << "\n";
    }

    std::cout << "Get 4: " << cache.get(4) << "\n"; // должен быть доступен

    cache.erase(1); // удалим ключ 1
    try {
        cache.get(1); // должен кинуть исключение
    }
    catch (const std::exception& ex) {
        std::cout << "Exception caught after erase: " << ex.what() << "\n";
    }

    return 0;
}
