#include <type_traits>
#include <cxxabi.h>

#include "JSON.hpp"
#include "Error.hpp"

namespace autojson {

// operators | and || for giving a default value to invaild JSONs
template<typename T>
JSON& JSON::operator||(const T &rhs) {
    if (this->type == JSONType::INVALID) {
        *this = JSON(rhs);
    }
    return *this;
}

template<typename T>
JSON& JSON::operator||(T &&rhs) {
    if (this->type == JSONType::INVALID) {
        *this = JSON(std::move(rhs));
    }
    return *this;
}

template<typename T>
JSON& JSON::operator|(const T &rhs) {
    return *this || rhs;
}

template<typename T>
JSON& JSON::operator|(T &&rhs) {
    return *this || std::move(rhs);
}

// deep copy pointers
template<typename T>
JSON::JSON(const T *x) : type(INVALID), content(nullptr) {
    if (x != nullptr) {
        *this = JSON(*x);
    }
}

template<typename T>
JSON::JSON(T *x) : type(INVALID), content(nullptr) {
    if (x != nullptr) {
        *this = JSON(*x);
    }
}

// small STL constructors
template<typename Type>
JSON::JSON(const std::vector<Type> &els) : type(JSONType::VECTOR) {
    auto v = new std::vector<JSON>;

    for (const Type& itr : els) {
        v->emplace_back(JSON(itr));
    }

    this->content = v;
}

template<typename T>
JSON::JSON(const std::map<std::string, T> &els) : type(JSONType::OBJECT) {
    auto m = new std::map<std::string, JSON>;

    for (const auto& itr : els) {
        (*m)[itr.first] = JSON(itr.second);
    }
    this->content = m;
}

template<typename Type>
JSON::operator std::vector<Type>() {
    this->checkType(JSONType::VECTOR);
    std::vector<Type> v;
    for (JSON& itr : (*this)) {
        v.emplace_back(itr.get<Type>());
    }
    return v;
}

template<typename Type>
JSON::operator std::vector<Type>() const {
    this->checkType(JSONType::VECTOR);
    std::vector<Type> v;
    for (const JSON& itr : (*this)) {
        v.emplace_back(itr.get<Type>());
    }
    return v;
}

template<typename Type>
JSON& JSON::push_back(const Type &rhs) {
    this->checkType(JSONType::VECTOR);
    auto& v = *(std::vector<JSON>*)(this->content);
    v.push_back(rhs);
    return v.back();
}

template<typename Type>
JSON& JSON::emplace_back(Type &&rhs) {
    this->checkType(JSONType::VECTOR);
    auto& v = *(std::vector<JSON>*)(this->content);
    v.emplace_back(rhs);
    return v.back();
}

template<typename Type>
JSON::operator std::map<std::string, Type>() {
    std::map<std::string, Type> m;
    auto& mp = *(std::map<std::string, JSON>*)(content);
    for (auto& itr : mp) {
        m[itr.first] = itr.second.get<Type>();
    }
    return m;
}

template<typename Type>
JSON::operator std::map<std::string, Type>() const {
    std::map<std::string, Type> m;
    auto& mp = *(std::map<std::string, JSON>*)(content);
    for (const auto& itr : mp) {
        m[itr.first] = itr.second.get<Type>();
    }
    return m;
}

template<typename T>
T JSON::get() {
    return T(*this);
}

template<typename T>
T JSON::get() const {
    return T(*this);
}

}  // namespace AutoJson
