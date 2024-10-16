#define DOCTEST_CONFIG_IMPLEMENT

#include "pairinteraction/tools/test.hpp"

#include "pairinteraction/database/Database.hpp"
#include "pairinteraction/utils/paths.hpp"

#include <cstdlib>
#include <doctest/doctest.h>
#include <filesystem>
#include <httplib.h>
#include <mutex>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace pairinteraction {
int test(int argc, char **argv, bool download_missing, std::filesystem::path database_dir) {

    // Configure a logger for the tests
    std::filesystem::path logdir = paths::get_pairinteraction_cache_directory() / "logs";

    if (!std::filesystem::exists(logdir)) {
        std::filesystem::create_directories(logdir);
    } else if (!std::filesystem::is_directory(logdir)) {
        throw std::runtime_error("Log path is not a directory.");
    }

    std::filesystem::path logfile = logdir / "test.log";

    static std::once_flag flag_doctest_logger;
    std::call_once(flag_doctest_logger, [&logfile] {
        auto console_sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(spdlog::color_mode::always);
        auto file_sink =
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile.string(), true);
        auto doctest_logger =
            std::make_shared<spdlog::logger>(spdlog::logger("doctest", {console_sink, file_sink}));
        doctest_logger->set_pattern("[doctest] [%Y-%m-%d %H:%M:%S.%e %t] [%^%l%$] [%s:%#] %v");
        spdlog::register_logger(doctest_logger);

        const char *log_level = std::getenv("SPDLOG_LEVEL");
        if (log_level != nullptr) {
            spdlog::set_level(spdlog::level::from_str(log_level));
        }
    });

    // Setup the tests
    doctest::Context ctx;
    ctx.setOption("abort-after", 5);
    ctx.setOption("no-run", 0);
    ctx.setOption("force-colors", true);
    ctx.applyCommandLine(argc, argv);
    ctx.setOption("no-breaks", true);

    // Create a global database instance
    Database::get_global_instance(download_missing, false, std::move(database_dir));

    // Run the tests
    int exitcode = ctx.run();

    if (exitcode != 0) {
        if (download_missing) {
            httplib::Client client("https://www.github.com");
            auto res = client.Head("/");
            if (!res) {
                SPDLOG_ERROR(
                    "Test failed. Please check your internet connection. An internet "
                    "connection is required to download databases of atomic states and matrix "
                    "elements if they are not available locally.");
            } else {
                SPDLOG_ERROR(
                    "Tests failed. Consider creating an issue on "
                    "https://github.com/pairinteraction/pairinteraction/issues, attaching the "
                    "log file {}.",
                    logfile.string());
            }
        } else {
            SPDLOG_ERROR("Tests failed. If the tests failed because of unavailable states or "
                         "matrix elements, consider downloading missing databases by calling "
                         "the test function with 'download_missing = true'.");
        }
    }

    return exitcode;
};
} // namespace pairinteraction
