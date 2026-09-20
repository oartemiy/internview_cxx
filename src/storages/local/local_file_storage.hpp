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

    /**
     * @brief Write(or rewrite) file on disk
     *
     * @param path
     * @param contents
     * @throw std::runtime_error
     */
    std::string Save(const utils::File& file) override;

    /**
     * @brief Read file from disk and returns it's bytes as std::string
     *
     * @param path
     * @return std::string
     * @throw std::runtime_error
     */
    std::string Load(const std::string& key) override;

    /**
     * @brief Checks is file on disk
     *
     * @param key
     * @return true
     * @return false
     */
    bool Exists(const std::string& key) override;

    /**
     * @brief Deleting file from disk
     *
     * @param path
     */
    void Delete(const std::string& key) override;

private:
    std::string GenerateFileKey(const utils::File& file);

    std::string FullPath(const std::string& key);

    userver::engine::TaskProcessor& fs_tp_;
    std::string base_dir_;
    std::vector<std::string> supported_extensions_;
};
}  // namespace internview::storages::local
