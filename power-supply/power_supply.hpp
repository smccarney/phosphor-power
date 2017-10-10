#pragma once
#include <sdbusplus/bus/match.hpp>
#include "device.hpp"
#include "pmbus.hpp"
#include "timer.hpp"
#include "names_values.hpp"

namespace witherspoon
{
namespace power
{
namespace psu
{

namespace sdbusRule = sdbusplus::bus::match::rules;

/**
 * @class PowerSupply
 * Represents a PMBus power supply device.
 */
class PowerSupply : public Device
{
    public:
        PowerSupply() = delete;
        PowerSupply(const PowerSupply&) = delete;
        PowerSupply(PowerSupply&&) = default;
        PowerSupply& operator=(const PowerSupply&) = default;
        PowerSupply& operator=(PowerSupply&&) = default;
        ~PowerSupply() = default;

        /**
         * Constructor
         *
         * @param[in] name - the device name
         * @param[in] inst - the device instance
         * @param[in] objpath - the path to monitor
         * @param[in] invpath - the inventory path to use
         * @param[in] bus - D-Bus bus object
         * @param[in] e - event object
         * @param[in] t - time to allow power supply to assert PG#
         */
        PowerSupply(const std::string& name, size_t inst,
                    const std::string& objpath,
                    const std::string& invpath,
                    sdbusplus::bus::bus& bus,
                    event::Event& e,
                    std::chrono::seconds& t);

        /**
         * Power supply specific function to analyze for faults/errors.
         *
         * Various PMBus status bits will be checked for fault conditions.
         * If a certain fault bits are on, the appropriate error will be
         * committed.
         */
        void analyze() override;

        /**
         * Write PMBus CLEAR_FAULTS
         *
         * This function will be called in various situations in order to clear
         * any fault status bits that may have been set, in order to start over
         * with a clean state. Presence changes and power state changes will
         * want to clear any faults logged.
         */
        void clearFaults() override;

    private:
        /**
         * The path to use for reading various PMBus bits/words.
         */
        std::string monitorPath;

        /**
         * @brief Pointer to the PMBus interface
         *
         * Used to read out of or write to the /sysfs tree(s) containing files
         * that a device driver monitors the PMBus interface to the power
         * supplies.
         */
        witherspoon::pmbus::PMBus pmbusIntf;

        /**
         * @brief D-Bus path to use for this power supply's inventory status.
         */
        std::string inventoryPath;

        /** @brief Connection for sdbusplus bus */
        sdbusplus::bus::bus& bus;

        /** @brief True if the power supply is present. */
        bool present = false;

        /** @brief Used to subscribe to D-Bus property changes for Present */
        std::unique_ptr<sdbusplus::bus::match_t> presentMatch;

        /** @brief True if the power is on. */
        bool powerOn = false;

        /** @brief True if power on fault has been detected/reported. */
        bool powerOnFault = false;

        /** @brief The sd_event structure used by the power on timer. */
        event::Event& event;

        /**
         * @brief Interval to setting powerOn to true.
         *
         * The amount of time to wait from power state on to setting the
         * internal powerOn state to true. The amount of time the power supply
         * is allowed to delay setting DGood/PG#.
         */
        std::chrono::seconds powerOnInterval;

        /**
         * @brief Timer used to delay setting the internal powerOn state.
         *
         * The timer used to do the callback after the power state has been on
         * long enough.
         */
        Timer powerOnTimer;

        /** @brief Used to subscribe to D-Bus power on state changes */
        std::unique_ptr<sdbusplus::bus::match_t> powerOnMatch;

        /** @brief Has a PMBus read failure already been logged? */
        bool readFailLogged = false;

        /**
         * @brief Set to true when a VIN UV fault has been detected
         *
         * This is the VIN_UV_FAULT bit in the low byte from the STATUS_WORD
         * command response.
         */
        bool vinUVFault = false;

        /**
         * @brief Set to true when an input fault or warning is detected
         *
         * This is the "INPUT FAULT OR WARNING" bit in the high byte from the
         * STATUS_WORD command response.
         */
        bool inputFault = false;

        /**
         * @brief Set to true when an output over current fault is detected
         *
         * This is the "IOUT_OC_FAULT" bit in the low byte from the STATUS_WORD
         * command response.
         */
        bool outputOCFault = false;

        /**
         * @brief Set to true when the output overvoltage fault is detected
         */
        bool outputOVFault = false;

        /**
         * @brief Set to true when a fan fault or warning condition is detected
         */
        bool fanFault = false;

        /**
         * @brief Set to true during a temperature fault or warn condition.
         */
        bool temperatureFault = false;

        /**
         * @brief Callback for inventory property changes
         *
         * Process change of Present property for power supply.
         *
         * @param[in]  msg - Data associated with Present change signal
         *
         */
        void inventoryChanged(sdbusplus::message::message& msg);

        /**
         * Updates the presence status by querying D-Bus
         *
         * The D-Bus inventory properties for this power supply will be read to
         * determine if the power supply is present or not and update this
         * objects present member variable to reflect current status.
         */
        void updatePresence();

        /**
         * @brief Updates the poweredOn status by querying D-Bus
         *
         * The D-Bus property for the system power state will be read to
         * determine if the system is powered on or not.
         */
        void updatePowerState();

        /**
         * @brief Callback for power state property changes
         *
         * Process changes to the powered on stat property for the system.
         *
         * @param[in] msg - Data associated with the power state signal
         */
        void powerStateChanged(sdbusplus::message::message& msg);

        /**
         * @brief Wrapper for PMBus::read() and adding metadata
         *
         * @param[out] nv - NamesValues instance to store cmd string and value
         * @param[in] cmd - String for the command to read data from.
         * @param[in] type - The type of file to read the command from.
         */
        void captureCmd(util::NamesValues& nv, const std::string& cmd,
                        witherspoon::pmbus::Type type);

        /**
         * @brief Checks for input voltage faults and logs error if needed.
         *
         * Check for voltage input under voltage fault (VIN_UV_FAULT) and/or
         * input fault or warning (INPUT_FAULT), and logs appropriate error(s).
         *
         * @param[in] statusWord  - 2 byte STATUS_WORD value read from sysfs
         */
        void checkInputFault(const uint16_t statusWord);

        /**
         * @brief Checks for power good negated or unit is off in wrong state
         *
         * @param[in] statusWord  - 2 byte STATUS_WORD value read from sysfs
         */
        void checkPGOrUnitOffFault(const uint16_t statusWord);

        /**
         * @brief Checks for output current over current fault.
         *
         * IOUT_OC_FAULT is checked, if on, appropriate error is logged.
         *
         * @param[in] statusWord  - 2 byte STATUS_WORD value read from sysfs
         */
        void checkCurrentOutOverCurrentFault(const uint16_t statusWord);

        /**
         * @brief Checks for output overvoltage fault.
         *
         * VOUT_OV_FAULT is checked, if on, appropriate error is logged.
         *
         * @param[in] statusWord  - 2 byte STATUS_WORD value read from sysfs
         */
        void checkOutputOvervoltageFault(const uint16_t statusWord);

        /**
         * @brief Checks for a fan fault or warning condition.
         *
         * The high byte of STATUS_WORD is checked to see if the "FAN FAULT OR
         * WARNING" bit is turned on. If it is on, log an error.
         *
         * @param[in] statusWord - 2 byte STATUS_WORD value read from sysfs
         */
        void checkFanFault(const uint16_t statusWord);

        /**
         * @brief Checks for a temperature fault or warning condition.
         *
         * The low byte of STATUS_WORD is checked to see if the "TEMPERATURE
         * FAULT OR WARNING" bit is turned on. If it is on, log an error,
         * call out the power supply indicating the fault/warning condition.
         *
         * @parma[in] statusWord - 2 byte STATUS_WORD value read from sysfs
         */
        void checkTemperatureFault(const uint16_t statusWord);

};

}
}
}