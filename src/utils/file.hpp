#pragma once

#include <string>
#include <string_view>
#include <userver/server/http/form_data_arg.hpp>

namespace internview::utils {

struct File {
    std::string_view data;
    std::string filename;
    std::optional<std::string> content_type;

    File(const userver::server::http::FormDataArg& form_data_arg)
        : data(form_data_arg.value),
          filename(form_data_arg.filename ? *form_data_arg.filename : ""),
          content_type(form_data_arg.content_type) {
    }
};
}  // namespace internview::utils
