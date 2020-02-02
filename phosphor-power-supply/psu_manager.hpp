#pragma once

#include <sdbusplus/bus/match.hpp>
#include <sdeventplus/event.hpp>
#include <sdeventplus/utility/timer.hpp>

namespace phosphor
{
namespace power
{
namespace manager
{

/**
 * @class PSUManager
 *
 * This class will create an object used to manage and monitor a list of power
 * supply devices.
 */
class PSUManager
{
  public:
    PSUManager() = delete;
    ~PSUManager() = default;
    PSUManager(const PSUManager&) = delete;
    PSUManager& operator=(const PSUManager&) = delete;
    PSUManager(PSUManager&&) = delete;
    PSUManager& operator=(PSUManager&&) = delete;

    /**
     * Constructor
     *
     * @param[in] bus - D-Bus bus object
     * @param[in] e - event object
     * @param[in] i - polling interval in milliseconds
     */
    PSUManager(sdbusplus::bus::bus& bus, const sdeventplus::Event& e,
               std::chrono::milliseconds i) :
        bus(bus),
        timer(e, std::bind(&PSUManager::analyze, this), i)
    {
    }

    /**
     * Initializes the manager.
     *
     * Get current BMC state, ...
     */
    void initialize()
    {
    }

    /**
     * Starts the timer to start monitoring the list of devices.
     */
    int run()
    {
        return timer.get_event().loop();
    }

    /**
     * This function will be called in various situations in order to clear
     * any fault status bits that may have been set, in order to start over
     * with a clean state. Presence changes and power state changes will want
     * to clear any faults logged.
     */
    void clearFaults()
    {
    }

  private:
    /**
     * The D-Bus object
     */
    sdbusplus::bus::bus& bus;

    /**
     * The timer that runs to periodically check the power supplies.
     */
    sdeventplus::utility::Timer<sdeventplus::ClockId::Monotonic> timer;

    /**
     * Analyze the status of each of the power supplies.
     */
    void analyze()
    {
    }

    /** @brief True if the power is on. */
    bool powerOn = false;

    /** @brief Used to subscribe to D-Bus power on state changes */
    std::unique_ptr<sdbusplus::bus::match_t> powerOnMatch;

    /**
     * @brief Updates the poweredOn status by querying D-Bus
     *
     * The D-Bus property for the system power state will be read to determine
     * if the system is powered on or not.
     */
    void updatePowerState();

    /**
     * @brief Callback for power state property changes
     *
     * Process changes to the powered on state property for the system.
     *
     * @param[in] msg - Data associated with the power state signal
     */
    void powerStateChanged(sdbusplus::message::message& msg);

    /**
     * @brief Adds properties to the inventory.
     *
     * Reads the values from the devices and writes them to the associated
     * power supply D-Bus inventory objects.
     *
     * This needs to be done on startup, and each time the presence state
     * changes.
     */
    void updateInventory();
};

} // namespace manager
} // namespace power
} // namespace phosphor