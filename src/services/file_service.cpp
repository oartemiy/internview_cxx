#include "file_service.hpp"

#include "userver/components/component_fwd.hpp"
#include "userver/fs/read.hpp"
#include "userver/fs/write.hpp"

namespace internview::services {

FileService::FileService(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& component_context)
    : fs_tp_(userver::components::GetFsTaskProcessor(
          config,
          component_context))
{
    EnsureDirectories();
}

void FileService::EnsureDirectories() {
    userver::fs::CreateDirectories(fs_tp_, pdf_folder, boost::filesystem::perms::owner_all);
    userver::fs::CreateDirectories(fs_tp_, img_folder, boost::filesystem::perms::owner_all);
}

void FileService::WriteFile(const std::string& path, std::string_view contents) {
    userver::fs::RewriteFileContentsAtomically(
        fs_tp_, path, contents,
        boost::filesystem::perms::owner_read | boost::filesystem::perms::owner_write);
}

std::string FileService::ReadFile(const std::string& path) {
    return userver::fs::ReadFileContents(fs_tp_, path);
}

}  // namespace internview::services
