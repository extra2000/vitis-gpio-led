/*
 * mygpio.hpp
 */

#ifndef INCLUDE_MYGPIO_HPP_
#define INCLUDE_MYGPIO_HPP_


#include <gpiod.hpp>
#include <logger.hpp>

namespace mygpio
{

namespace led
{

enum state
{
    ON,
    OFF
};

class MyLED
{
protected:
    std::string consumer;
    std::string chipname;
    int line_offset;
    gpiod::chip *chip;
    logger::Logger log;
public:
    MyLED(const std::string& consumer, const std::string& chipname);
    ~MyLED();
    void set_state(state led_state, int line_offset);
}; // class MyLED

} // namespace state

} // namespace mygpio


#endif // INCLUDE_MYGPIO_HPP_
