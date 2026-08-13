#pragma once

#include <stdexcept>
#include <string>

namespace internview::errors {

class NotFoundError : public std::runtime_error {
public:
    explicit NotFoundError(const std::string& msg) : std::runtime_error(msg) {
    }
};

class ConflictError : public std::runtime_error {
public:
    explicit ConflictError(const std::string& msg) : std::runtime_error(msg) {
    }
};

class SystemError : public std::runtime_error {
public:
    explicit SystemError(const std::string& msg) : std::runtime_error(msg) {
    }
};

class InvalidPasswordError : public std::runtime_error {
public:
    explicit InvalidPasswordError(const std::string& msg) : std::runtime_error(msg) {
    }
};

class FileUploadError : public std::runtime_error {
public:
    explicit FileUploadError(const std::string& msg) : std::runtime_error(msg) {
    }
};

}  // namespace internview::errors
