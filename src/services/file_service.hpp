#pragma once

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

    /**
     * @brief Write(or rewrite) file on disk
     *
     * @param path
     * @param contents
     * @throw std::runtime_error
     */
    void WriteFile(const std::string& path, std::string_view contents);

    /**
     * @brief Read file from disk and returns it's bytes as std::string
     *
     * @param path
     * @return std::string
     * @throw std::runtime_error
     */
    std::string ReadFile(const std::string& path);

private:
    /**
     * @brief Initialize files dirictories
     * @throw std::runtime_error
     *
     */
    void EnsureDirectories();

    userver::engine::TaskProcessor& fs_tp_;
};

}  // namespace internview::services
