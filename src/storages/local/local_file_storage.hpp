#pragma once

#include <string>
#include <vector>

#include "storages/interfaces/file_storage.hpp"
#include "userver/engine/task/task_processor_fwd.hpp"

namespace internview::storages::local {

class LocalFileStorage final : public internview::storages::interfaces::IFileStorage {
public:
    explicit LocalFileStorage(userver::engine::TaskProcessor& file_task_processor,
                              std::string base_dir, std::vector<std::string> supported_extensions);

    ~LocalFileStorage() override = default;

    LocalFileStorage(const LocalFileStorage&) = delete;

    LocalFileStorage& operator=(const LocalFileStorage&) = delete;

    LocalFileStorage(LocalFileStorage&&) = delete;

    LocalFileStorage& operator=(LocalFileStorage&&) = delete;

    std::string Save(const File& file) override;

    std::string Load(const std::string& key) override;

    bool Exists(const std::string& key) override;

    void Delete(const std::string& key) override;

private:
    std::string GenerateFileKey(const File& file);

    std::string FullPath(const std::string& key);

    userver::engine::TaskProcessor& fs_tp_;
    std::string base_dir_;
    std::vector<std::string> supported_extensions_;
};
}  // namespace internview::storages::local
