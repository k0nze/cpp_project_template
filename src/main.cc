#include <iostream>
#include <memory>
#include <unordered_map>

class Animal {
public:
    Animal(const std::string& name) : name_(name){};

    std::string get_name() { return name_; }

    virtual void make_noise() { std::cout << "nope" << std::endl; }

private:
    std::string name_;
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

struct Deref {
    struct Hash {
        template <typename T>
        std::size_t operator()(const std::shared_ptr<T>& p) const {
            return std::hash<T>()(*p);
        }
    };
    struct Compare {
        template <typename T>
        std::size_t operator()(const std::shared_ptr<T>& a,
                               const std::shared_ptr<T>& b) const {
            return *a == *b;
        }
    };
};

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T1>{}(p.second);

        return h1 ^ h2;
    }
};

int main(int argc, char** argv) {
    auto m0 =
        std::unordered_map<std::pair<std::shared_ptr<Animal>, std::shared_ptr<Animal>>,
                           int, PairHash>();

    auto fritz = std::make_shared<HouseCat>("Fritz");
    auto bello = std::make_shared<Shepard>("Bello");

    auto key = std::make_pair(fritz, bello);

    m0.insert(std::make_pair(key, 1));

    if (m0.find(key) != m0.end()) {
        std::cout << m0.at(key) << std::endl;
    }

    return 0;
}
