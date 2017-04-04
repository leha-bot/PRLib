#include "logger.hpp"
#include <QTime>
#include <QDate>


std::shared_ptr<spdlog::logger> logger;

std::string createLogFilename(const std::string& workingDir)
{
    return workingDir
           + QDate::currentDate().toString("'data_'yyyy_MM_dd_").toStdString()
           + QTime::currentTime().toString("hh_mm_ss'.txt'").toStdString();
}
