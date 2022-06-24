/*
 * mygpio.cpp
 */

#include <boost/format.hpp>
#include <mygpio.hpp>

namespace mygpio
{

namespace led
{

MyLED::MyLED(const std::string& consumer, const std::string& chipname)
{
    log.init("myled");

    this->consumer = consumer;
    this->chipname = chipname;
    chip = new gpiod::chip(chipname);

    log.debug(boost::format("Consumer = '%1%', Engine initialized with chipname '%2%'") % this->consumer % this->chipname);
}

MyLED::~MyLED()
{
    if (chip != NULL)
    {
        log.debug("Chip was initialized. Destroying chip ...");
        delete chip;
    }
    log.debug("Engine destroyed");
}

void MyLED::set_state(state led_state, int line_offset)
{
    gpiod::line line = chip->get_line(line_offset);

    line.request({consumer, gpiod::line_request::DIRECTION_OUTPUT, 0});

    switch(led_state)
    {
    case state::ON:
        line.set_value(1);
        log.debug("LED state set to ON");
        break;
    case state::OFF:
        line.set_value(0);
        log.debug("LED state set to OFF");
        break;
    default:
        throw(std::invalid_argument("Invalid LED state"));
    }
}

} // namespace led

} // namespace mygpio
