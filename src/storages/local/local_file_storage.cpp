#include "local_file_storage.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <utility>
#include <vector>

#include "userver/fs/read.hpp"
#include "userver/fs/write.hpp"
#include "userver/logging/log.hpp"
#include "userver/server/handlers/exceptions.hpp"
#include "userver/utils/uuid4.hpp"

namespace internview::storages::local {

LocalFileStorage::LocalFileStorage(userver::engine::TaskProcessor& file_task_processor,
                                   std::string base_dir,
                                   std::vector<std::string> supported_extansions)
    : fs_tp_(file_task_processor),
      base_dir_(std::move(base_dir)),
      supported_extensions_(std::move(supported_extansions)) {
    userver::fs::CreateDirectories(fs_tp_, base_dir_, boost::filesystem::perms::owner_all);
}

std::string LocalFileStorage::GenerateFileKey(const File& file) {
    auto ext = std::filesystem::path(file.filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext.empty()) {
        LOG_WARNING() << "Empty extension";
        throw userver::server::handlers::ClientError(

            userver::formats::json::MakeObject("message", "Empty file extension"));
    }
    if (auto it = std::ranges::find(supported_extensions_, ext);
        it == supported_extensions_.end()) {
        // TODO: add more info
        LOG_WARNING() << "Invalid extension";

        throw userver::server::handlers::ClientError(
            userver::formats::json::MakeObject("message", "Invalid file extension"));
    }
    std::string id = userver::utils::generators::GenerateUuid();
    std::string name = id + ext;
    return name;
}

std::string LocalFileStorage::FullPath(const std::string& key) {
    return (std::filesystem::path(base_dir_) / key).string();
}

std::string LocalFileStorage::Save(const File& file) {
    auto file_key = GenerateFileKey(file);
    auto path = FullPath(file_key);
    LOG_INFO() << "New file:" << path;
    userver::fs::RewriteFileContentsAtomically(
        fs_tp_, path, file.data,
        boost::filesystem::perms::owner_write | boost::filesystem::perms::owner_read);
    return file_key;
}

std::string LocalFileStorage::Load(const std::string& key) {
    auto path = FullPath(key);
    return userver::fs::ReadFileContents(fs_tp_, path);
}

bool LocalFileStorage::Exists(const std::string& key) {
    auto path = FullPath(key);
    return userver::fs::FileExists(fs_tp_, path);
}

void LocalFileStorage::Delete(const std::string& key) {
    auto path = FullPath(key);
    userver::fs::RemoveSingleFile(fs_tp_, path);
}

}  // namespace internview::storages::local
