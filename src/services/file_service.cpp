#include "file_service.hpp"

#include <exception>

#include "userver/components/component_fwd.hpp"
// #include "userver/components/fs_cache.hpp"
#include "userver/fs/read.hpp"
#include "userver/fs/write.hpp"
#include "userver/logging/log.hpp"

namespace internview::services {

FileService::FileService(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& component_context)
    : fs_tp_(userver::components::GetFsTaskProcessor(config, component_context)) /*,
                                                                                    fs_cache_(component_context.FindComponent<userver::components::FsCache>("fs-cache-uploads"))
                                                                                  */
{
    EnsureDirectories();
}

void FileService::EnsureDirectories() {
    try {
        userver::fs::CreateDirectories(fs_tp_, pdf_folder, boost::filesystem::perms::owner_all);
        userver::fs::CreateDirectories(fs_tp_, img_folder, boost::filesystem::perms::owner_all);
    } catch (const std::exception& e) {
        LOG_ERROR() << "Failed to create directories: " << e.what();
        throw std::runtime_error("Could not initialize file storage");
    }
}

bool FileService::WriteFile(const std::string& path, std::string_view contents) {
    try {
        userver::fs::RewriteFileContentsAtomically(
            fs_tp_, path, contents,
            boost::filesystem::perms::owner_read | boost::filesystem::perms::owner_write);
        return true;
    } catch (std::exception& e) {
        LOG_ERROR() << "WriteFile failed for " << path << ": " << e.what();
        return false;
    }
}

std::optional<std::string> FileService::ReadFile(const std::string& path) {
    try {
        if (!userver::fs::FileExists(fs_tp_, path)) {
            LOG_DEBUG() << "File not found: " << path;
            return std::nullopt;
        }
        // return fs_cache_.GetClient().TryGetFile(path)->data;
        return userver::fs::ReadFileContents(fs_tp_, path);
    } catch (const std::exception& e) {
        LOG_ERROR() << "ReadFile failed: " << e.what();
        throw;  // пробрасываем дальше
    }
}

}  // namespace internview::services
