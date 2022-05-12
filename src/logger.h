#ifndef WS_TO_MONGO_LOGGER_H
#define WS_TO_MONGO_LOGGER_H

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;

void init_logging(std::string filename){

    logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
    logging::add_file_log(
            keywords::file_name = filename,
            keywords::format = "[%TimeStamp%] [%Severity%] [%LineID%] %Message%",
            keywords::rotation_size = 10 * 1024 * 1024
    );
    logging::core::get()->set_filter
            (
                    logging::trivial::severity >= logging::trivial::info
            );

    logging::add_common_attributes();
}

#endif //WS_TO_MONGO_LOGGER_H
