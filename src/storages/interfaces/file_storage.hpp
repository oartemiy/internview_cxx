#pragma once

#include <string>

#include "utils/common_file_storage.hpp"

namespace internview::storages::interfaces {

class IFileStorage {
public:
    virtual ~IFileStorage() = default;
    
    virtual std::string Save(const File& file) = 0;

    virtual std::string Load(const std::string& key) = 0;

    virtual bool Exists(const std::string& key) = 0;

    virtual void Delete(const std::string& key) = 0;
};

}  // namespace internview::storages::interfaces
