#pragma once

#include <utility>

namespace drip {

template<typename T, typename E>
class Result {
public:
    Result(T val) : value_(std::move(val)), has_value_(true) {}
    Result(E err) : error_(std::move(err)), has_value_(false) {}

    bool has_value() const { return has_value_; }
    T& value() { return value_; }
    const T& value() const { return value_; }
    E& error() { return error_; }
    const E& error() const { return error_; }
    T& operator*() & { return value_; }
    const T& operator*() const& { return value_; }
    T&& operator*() && { return std::move(value_); }

private:
    T value_;
    E error_;
    bool has_value_;
};

}
