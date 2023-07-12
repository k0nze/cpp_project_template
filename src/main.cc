#include <iostream>
#include <memory>
#include <unordered_map>

class Animal {
public:
    Animal(const std::string& name) : name(name){};

    std::string get_name() { return name; }

    virtual void make_noise() { std::cout << "nope" << std::endl; }

    bool operator==(Animal& animal) {
        std::cout << "call: operator==" << std::endl;
        return name.compare(animal.get_name()) == 0;
    }

    std::string name;
};

class Feline : public Animal {
public:
    Feline(const std::string& name) : Animal(name){};
};

class HouseCat : public Feline {
public:
    HouseCat(const std::string& name) : Feline(name){};

    virtual void make_noise() { std::cout << "meow" << std::endl; }
};

class Canine : public Animal {
public:
    Canine(const std::string& name) : Animal(name){};
};

class Shepard : public Canine {
public:
    Shepard(const std::string& name) : Canine(name){};

    virtual void make_noise() { std::cout << "ruff" << std::endl; }
};

class MapKey {
public:
    MapKey(std::shared_ptr<Animal> first, std::shared_ptr<Animal> second)
        : first(first), second(second){};
    std::shared_ptr<Animal> first;
    std::shared_ptr<Animal> second;

    bool operator==(const MapKey& map_key) const {
        // std::cout << "call MapKey.operator==" << std::endl;
        auto x0 = map_key.first->name.compare(first->name) == 0;
        auto x1 = map_key.second->name.compare(second->name) == 0;

        return x0 & x1;
    }
};

template <>
struct std::hash<Animal> {
    std::size_t operator()(Animal const& a) const {
        // std::cout << "call: Hash" << std::endl;
        return std::hash<std::string>{}(a.name);
    }
};

template <>
struct std::hash<std::shared_ptr<Animal>> {
    std::size_t operator()(std::shared_ptr<Animal> const& a) const {
        // std::cout << "call: Hash shared_ptr" << std::endl;
        return std::hash<Animal>{}(*a);
    }
};

template <>
struct std::hash<MapKey> {
    std::size_t operator()(MapKey const& mk) const {
        auto h0 = std::hash<std::shared_ptr<Animal>>{}(mk.first);
        auto h1 = std::hash<std::shared_ptr<Animal>>{}(mk.second);

        return h0 ^ h1;
    }
};

struct Deref {
    struct Hash {
        template <typename T>
        std::size_t operator()(const std::shared_ptr<T>& p) const {
            // std::cout << "call: Deref" << std::endl;
            return std::hash<T>()(*p);
        }
    };
    struct Compare {
        template <typename T>
        std::size_t operator()(const std::shared_ptr<T>& a,
                               const std::shared_ptr<T>& b) const {
            // std::cout << "call: Compare" << std::endl;
            return *a == *b;
        }
    };
};

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h0 = std::hash<T1>{}(p.first);
        auto h1 = std::hash<T1>{}(p.second);

        // std::cout << "call: PairHash, " << h0 << "^" << h1 << " = " << (h0^h1) <<
        // std::endl;

        return h0 ^ h1;
    }
};

int main(int argc, char** argv) {
    auto m = std::unordered_map<MapKey, int>();

    auto a0 = std::make_shared<Animal>("a");
    auto a1 = std::make_shared<Animal>("a");

    auto b0 = std::make_shared<Animal>("b");
    auto b1 = std::make_shared<Animal>("b");

    auto key0 = MapKey(a0, b0);
    auto key1 = MapKey(a0, b0);

    std::cout << (key0 == key1) << std::endl;

    m.insert(std::make_pair(key0, 23));

    if (m.find(key0) != m.end()) {
        std::cout << "> found key0: " << m.at(key0) << std::endl;
    }

    if (m.find(key1) != m.end()) {
        std::cout << "> found key1: " << m.at(key1) << std::endl;
    }

    m.insert(std::make_pair(key1, 42));

    if (m.find(key1) != m.end()) {
        std::cout << "> found key1: " << m.at(key1) << std::endl;
    }

    if (m.find(key0) != m.end()) {
        std::cout << "> found key0: " << m.at(key0) << std::endl;
    }

    /*
    auto m =
        std::unordered_map<std::pair<std::shared_ptr<Animal>, std::shared_ptr<Animal>>,
                           int, PairHash>();

    auto a0 = std::make_shared<Animal>("a");
    auto a1 = std::make_shared<Animal>("a");

    auto b0 = std::make_shared<Animal>("b");
    auto b1 = std::make_shared<Animal>("b");

    //std::cout << (*a0 == *a1) << std::endl;

    auto hc0 = std::make_shared<HouseCat>("hc");
    auto hc1 = std::make_shared<HouseCat>("hc");

    //std::cout << (*hc0 == *hc1) << std::endl;

    auto key0 = std::make_pair(a0, b0);
    auto key1 = std::make_pair(a1, b1);

    m.insert({key0, 42});

    std::cout << "find key0" << std::endl;
    if(m.find(key0) != m.end()) {
        std::cout << "> found key0: " << m.at(key0) <<  std::endl;
    }

    std::cout << "find key1" << std::endl;
    if(m.find(key1) != m.end()) {
        std::cout << "> found key1: " << m.at(key1) <<  std::endl;
    }

    m.insert({key1, 23});

    if(m.find(key1) != m.end()) {
        std::cout << "> found key1: " << m.at(key1) <<  std::endl;
    }

    std::cout << "> found key0: " << m.at(key0) <<  std::endl;
    /*
    */

    // std::cout << std::hash<Animal>{}(*a0) << std::endl;
    // std::cout << std::hash<Animal>{}(*a1) << std::endl;
    //
    // std::cout << std::hash<std::shared_ptr<Animal>>{}(a0) << std::endl;
    // std::cout << std::hash<std::shared_ptr<Animal>>{}(a1) << std::endl;

    /*
    auto fritz0 = std::make_shared<HouseCat>("Fritz");
    auto bello0 = std::make_shared<Shepard>("Bello");

    auto key0 = std::make_pair(fritz0, bello0);

    m.insert(std::make_pair(key0, 1));

    auto key1 = std::make_pair(fritz0, bello0);

    auto fritz1 = std::make_shared<HouseCat>("Fritz");
    auto bello1 = std::make_shared<HouseCat>("Bello");

    auto key2 = std::make_pair(fritz1, bello1);

    if (m.find(key0) != m.end()) {
        std::cout << "key0: " << m.at(key0) << std::endl;
    }

    if (m.find(key1) != m.end()) {
        std::cout << "key1: " << m.at(key1) << std::endl;
    }

    if (m.find(key2) != m.end()) {
        std::cout << "key2: " << m.at(key1) << std::endl;
    }
    */

    return 0;
}
