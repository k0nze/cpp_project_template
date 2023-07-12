#include <iostream>
#include <memory>
#include <set>
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

int main(int argc, char** argv) {
    using key_type = std::pair<std::shared_ptr<Animal>, std::shared_ptr<Animal>>;

    auto equal = [](const key_type& a, const key_type& b) {
        return (a.first->name.compare(b.first->name) == 0) &&
               (a.second->name.compare(b.second->name) == 0);
    };

    auto hash = [](const key_type& p) {
        auto h0 = std::hash<std::shared_ptr<Animal>>{}(p.first);
        auto h1 = std::hash<std::shared_ptr<Animal>>{}(p.second);
        return h0 ^ h1;
    };

    auto m = std::unordered_map<key_type, int, decltype(hash), decltype(equal)>(8, hash,
                                                                                equal);

    auto a0 = std::make_shared<Animal>("a");
    auto a1 = std::make_shared<Animal>("a");

    auto b0 = std::make_shared<Animal>("b");
    auto b1 = std::make_shared<Animal>("b");

    m.insert({key_type(a0, b0), 42});

    if (m.find(key_type(a0, b0)) != m.end()) {
        std::cout << "> found a0,b0: " << m.at(key_type(a0, b0)) << std::endl;
    }

    if (m.find(key_type(a1, b1)) != m.end()) {
        std::cout << "> found a1,b1: " << m.at(key_type(a1, b1)) << std::endl;
    }

    return 0;
}
