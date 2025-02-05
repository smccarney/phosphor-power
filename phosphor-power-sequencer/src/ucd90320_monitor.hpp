#pragma once

#include "pmbus.hpp"
#include "power_sequencer_monitor.hpp"

#include <sdbusplus/bus.hpp>
#include <sdbusplus/bus/match.hpp>

namespace phosphor::power::sequencer
{

/**
 * @class UCD90320Monitor
 * This class implements fault analysis for the UCD90320
 * power sequencer device.
 */
class UCD90320Monitor : public PowerSequencerMonitor
{
  public:
    UCD90320Monitor() = delete;
    UCD90320Monitor(const UCD90320Monitor&) = delete;
    UCD90320Monitor& operator=(const UCD90320Monitor&) = delete;
    UCD90320Monitor(UCD90320Monitor&&) = delete;
    UCD90320Monitor& operator=(UCD90320Monitor&&) = delete;
    virtual ~UCD90320Monitor() = default;

    /**
     * Create a device object for UCD90320 monitoring.
     * @param[in] bus D-Bus bus object
     * @param[in] i2cBus The bus number of the power sequencer device
     * @param[in] i2cAddress The I2C address of the power sequencer device
     */
    UCD90320Monitor(sdbusplus::bus::bus& bus, std::uint8_t i2cBus,
                    const std::uint16_t i2cAddress);

    /**
     * Callback function to handle interfacesAdded D-Bus signals
     * @param msg Expanded sdbusplus message data
     */
    void interfacesAddedHandler(sdbusplus::message::message& msg);

  private:
    /**
     * The D-Bus bus object
     */
    sdbusplus::bus::bus& bus;

    /**
     * The match to Entity Manager interfaces added.
     */
    sdbusplus::bus::match_t match;

    /**
     * The read/write interface to this hardware
     */
    pmbus::PMBus pmbusInterface;

    /**
     * Finds the list of compatible system types using D-Bus methods.
     * This list is used to find the correct JSON configuration file for the
     * current system.
     */
    void findCompatibleSystemTypes();
};

} // namespace phosphor::power::sequencer
