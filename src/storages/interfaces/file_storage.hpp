#pragma once

#include <string>

#include "utils/file.hpp"

namespace internview::storages::interfaces {

class IFileStorage {
public:
    virtual ~IFileStorage() = default;

    virtual std::string Save(const utils::File& file) = 0;

    virtual std::string Load(const std::string& key) = 0;

    virtual bool Exists(const std::string& key) = 0;

    virtual void Delete(const std::string& key) = 0;
};

}  // namespace internview::storages::interfaces
