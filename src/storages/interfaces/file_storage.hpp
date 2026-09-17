#pragma once

#include <string>
#include "userver/server/http/form_data_arg.hpp"

namespace internview::storages::interfaces {

class IFileStorage {
public:
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

    virtual ~IFileStorage() = default;

    virtual std::string Save(const File& file) = 0;

    virtual std::string Load(const std::string& key) = 0;

    virtual bool Exists(const std::string& key) = 0;

    virtual void Delete(const std::string& key) = 0;
};

}  // namespace internview::storages::interfaces
