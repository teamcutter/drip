#pragma once

#include <string>
#include <ostream>
#include <utility>

namespace drip {

struct Error {
public:
    Error() = default;
    explicit Error(std::string message) : message_(std::move(message)) {}
    const std::string& get_message() const { return message_; }
private:
    std::string message_;
};

inline std::ostream& operator<<(std::ostream& os, const Error& err) {
    return os << err.get_message();
}

template<typename T>
class Result {
public:
    Result(T val) : value_(std::move(val)), has_value_(true) {}
    Result(Error err) : error_(std::move(err)), has_value_(false) {}

    bool has_value() const { return has_value_; }
    const T& value() const { return value_; }
    const Error& error() const { return error_; }
    T& operator*() & { return value_; }
    const T& operator*() const& { return value_; }
    T&& operator*() && { return std::move(value_); }

private:
    T value_;
    Error error_;
    bool has_value_;
};

}
