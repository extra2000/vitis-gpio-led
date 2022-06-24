#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <logger.hpp>
#include <mygpio.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    logger::LoggerConfig logcfg;
    logger::Logger log("main");
    std::string chipname;
    std::string state;
    int line_offset;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "show help message")
        ("verbose", "set log level to debug")
        ("chip", po::value<std::string>(&chipname)->required(), "GPIO chip name")
        ("line", po::value<int>(&line_offset)->required(), "GPIO pin offset number")
        ("state", po::value<std::string>(&state)->required(), "LED state")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc;
        return 0;
    }

    logger::severity_level log_severity_level;
    if (vm.count("verbose"))
    {
        log_severity_level = logger::severity_level::debug;
    }
    else
    {
        log_severity_level = logger::severity_level::info;
    }
    logcfg.set_min_severity("main", log_severity_level);
    logcfg.set_min_severity("myled", log_severity_level);

    mygpio::led::MyLED *myled = new mygpio::led::MyLED(argv[0], chipname);

    if (state == "on")
    {
        myled->set_state(mygpio::led::state::ON, line_offset);
    }
    else if (state == "off")
    {
        myled->set_state(mygpio::led::state::OFF, line_offset);
    }
    else
    {
        std::string msg = "Unknown LED state";
        log.error(msg);
        throw(std::invalid_argument(msg));
    }

    delete myled;

    return 0;
}
