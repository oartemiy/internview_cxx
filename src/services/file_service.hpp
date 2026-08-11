#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "userver/components/component_config.hpp"
#include "userver/components/component_context.hpp"

namespace internview::services {

class FileService {
public:
    inline static std::string pdf_folder = "/Users/oartemiy/code/internview/uploads/pdf/";
    inline static std::string img_folder = "/Users/oartemiy/code/internview/uploads/img/";

    FileService(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& component_context);

    bool WriteFile(const std::string& path, std::string_view contents);

    std::optional<std::string> ReadFile(const std::string& path);

private:
    void EnsureDirectories();

    userver::engine::TaskProcessor& fs_tp_;
};

}  // namespace internview::services
