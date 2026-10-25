#pragma once

#include <gmock/gmock.h>

#include "storages/interfaces/file_storage.hpp"

namespace internview::mocks {

class MockFileStorage : public storages::interfaces::IFileStorage {
public:
    MOCK_METHOD(std::string, Save, (const utils::File& file), (override));
    MOCK_METHOD(std::string, Load, (const std::string& key), (override));
    MOCK_METHOD(bool, Exists, (const std::string& key), (override));
    MOCK_METHOD(void, Delete, (const std::string& key), (override));
};

}  // namespace internview::mocks
