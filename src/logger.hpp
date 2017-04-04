#ifndef UFOCR_LOGGER_HPP
#define UFOCR_LOGGER_HPP

#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h"

#include <string>

extern std::shared_ptr<spdlog::logger> logger;

std::string createLogFilename(const std::string& workingDir);

#endif //UFOCR_LOGGER_HPP
