#include "storages/local/local_file_storage.hpp"

#include <userver/engine/task/current_task.hpp>
#include <userver/fs/blocking/read.hpp>
#include <userver/fs/blocking/temp_directory.hpp>
#include <userver/fs/blocking/write.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/form_data_arg.hpp>
#include <userver/utest/utest.hpp>

namespace {

using internview::storages::local::LocalFileStorage;
using userver::server::handlers::ClientError;

userver::server::http::FormDataArg MakeArg(std::string_view data, std::string filename) {
    userver::server::http::FormDataArg arg;
    arg.value = data;
    arg.filename = std::move(filename);
    return arg;
}

class LocalFileStorageTest : public ::testing::Test {
protected:
    std::string BaseDir() const {
        return temp_dir_.GetPath() + "/img";
    }

    LocalFileStorage MakeStorage() const {
        return LocalFileStorage(userver::engine::current_task::GetBlockingTaskProcessor(), BaseDir(),
                                {".png", ".jpg", ".jpeg"});
    }

    userver::fs::blocking::TempDirectory temp_dir_ = userver::fs::blocking::TempDirectory::Create();
};

}  // namespace

UTEST_F(LocalFileStorageTest, SaveLoadExistsDeleteRoundTrip) {
    auto storage = MakeStorage();
    const auto arg = MakeArg("\x89PNG-bytes", "avatar.png");

    const auto key = storage.Save(arg);

    EXPECT_TRUE(key.ends_with(".png"));
    EXPECT_TRUE(storage.Exists(key));
    EXPECT_EQ(storage.Load(key), "\x89PNG-bytes");

    storage.Delete(key);
    EXPECT_FALSE(storage.Exists(key));
}

UTEST_F(LocalFileStorageTest, KeyDoesNotContainOriginalFilename) {
    auto storage = MakeStorage();

    const auto key = storage.Save(MakeArg("data", "../../evil.png"));

    EXPECT_EQ(key.find('/'), std::string::npos);
    EXPECT_EQ(key.find("evil"), std::string::npos);
    EXPECT_TRUE(userver::fs::blocking::FileExists(BaseDir() + "/" + key));
}

UTEST_F(LocalFileStorageTest, ExtensionIsCaseInsensitiveAndNormalized) {
    auto storage = MakeStorage();

    const auto key = storage.Save(MakeArg("data", "Photo.JPG"));

    EXPECT_TRUE(key.ends_with(".jpg"));
}

UTEST_F(LocalFileStorageTest, GeneratesUniqueKeys) {
    auto storage = MakeStorage();

    const auto first = storage.Save(MakeArg("one", "a.png"));
    const auto second = storage.Save(MakeArg("two", "a.png"));

    EXPECT_NE(first, second);
    EXPECT_EQ(storage.Load(first), "one");
    EXPECT_EQ(storage.Load(second), "two");
}

UTEST_F(LocalFileStorageTest, RejectsFileWithoutExtension) {
    auto storage = MakeStorage();

    EXPECT_THROW(storage.Save(MakeArg("data", "avatar")), ClientError);
}

UTEST_F(LocalFileStorageTest, RejectsUnsupportedExtension) {
    auto storage = MakeStorage();

    EXPECT_THROW(storage.Save(MakeArg("data", "script.exe")), ClientError);
    EXPECT_THROW(storage.Save(MakeArg("data", "avatar.png.exe")), ClientError);
    EXPECT_THROW(storage.Save(MakeArg("data", "cv.pdf")), ClientError);
}

UTEST_F(LocalFileStorageTest, LoadOfMissingKeyThrows) {
    auto storage = MakeStorage();

    EXPECT_FALSE(storage.Exists("missing.png"));
    EXPECT_THROW(storage.Load("missing.png"), std::runtime_error);
}

// Keys come from DB columns, they must never resolve outside of base_dir
UTEST_F(LocalFileStorageTest, KeysCannotEscapeBaseDirectory) {
    auto storage = MakeStorage();
    const auto secret_path = temp_dir_.GetPath() + "/secret.txt";
    userver::fs::blocking::RewriteFileContents(secret_path, "top-secret");

    EXPECT_ANY_THROW(storage.Load("../secret.txt"));
    EXPECT_ANY_THROW(storage.Load(secret_path));
    EXPECT_FALSE(storage.Exists("../secret.txt"));

    EXPECT_ANY_THROW(storage.Delete("../secret.txt"));
    EXPECT_TRUE(userver::fs::blocking::FileExists(secret_path));
}
