#ifndef WS_TO_MONGO_LOGGER_H
#define WS_TO_MONGO_LOGGER_H

#include <string>
#include <iostream>
#include <fstream>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/support/date_time.hpp>


namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using boost::shared_ptr;

enum severity_level
{
    INFO,
    DEBUG,
    WARNING,
    ERROR,
    FATAL
};
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, severity_level lvl)
{
    static const char* const str[] =
            {
                    "INFO",
                    "DEBUG",
                    "WARNING",
                    "ERROR",
                    "FATAL"
            };
    if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast< int >(lvl);
    return strm;
}

src::severity_logger_mt< severity_level > lg;

void init_logging(std::string &filename){

    shared_ptr<std::ostream> strm(new std::ofstream(filename));
    shared_ptr<sinks::synchronous_sink<sinks::text_ostream_backend>> sink(
            new sinks::synchronous_sink< sinks::text_ostream_backend >);
    sink->locked_backend()->add_stream(strm);

    sink ->set_formatter(
            expr::format("%1% | %2% - %3%")
            % expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%d.%m.%Y %H:%M:%S")
            % expr::attr< severity_level >("Severity")
            % expr::smessage);

    logging::core::get()->add_sink(sink);
    logging::core::get()->add_global_attribute("ThreadID", attrs::current_thread_id());
    logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());


}

#endif //WS_TO_MONGO_LOGGER_H
