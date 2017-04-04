#include "logger.hpp"


std::shared_ptr<spdlog::logger> logger;

std::string createLogFilename(const std::string& workingDir)
{
    return workingDir;
}
