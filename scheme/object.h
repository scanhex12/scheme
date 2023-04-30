#pragma once

#include <memory>

class Object : public std::enable_shared_from_this<Object> {
public:
    bool is_quote = false;
    virtual ~Object() = default;
};

class Number : public Object {
public:
    Number(int x) : value_(x) {
    }
    int GetValue() const {
        return value_;
    }

private:
    int value_;
};

class Symbol : public Object {
public:
    Symbol(std::string symbol) : symbol_(symbol) {
    }
    const std::string& GetName() const {
        return symbol_;
    }

private:
    std::string symbol_;
};

class Cell : public Object {
public:
    Cell(std::shared_ptr<Object> fst, std::shared_ptr<Object> scd, std::shared_ptr<Object> par)
        : fst_(fst), scd_(scd), parent_(par) {
    }

    std::shared_ptr<Object> GetFirst() const {
        return fst_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return scd_;
    }
    std::shared_ptr<Object> GetParent() const {
        return parent_;
    }

    std::shared_ptr<Object>& GetFirst() {
        return fst_;
    }
    std::shared_ptr<Object>& GetSecond() {
        return scd_;
    }
    std::shared_ptr<Object>& GetParent() {
        return parent_;
    }

private:
    std::shared_ptr<Object> fst_;
    std::shared_ptr<Object> scd_;
    std::shared_ptr<Object> parent_;
};

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    auto x = std::dynamic_pointer_cast<T>(obj);
    if (!x) {
        return false;
    }
    return true;
}
