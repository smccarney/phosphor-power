/**
 * Copyright © 2020 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "device.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace phosphor::power::regulators
{

/**
 * @class Chassis
 *
 * A chassis within the system.
 *
 * Chassis are large enclosures that can be independently powered off and on by
 * the BMC.  Small and mid-sized systems may contain a single chassis.  In a
 * large rack-mounted system, each drawer may correspond to a chassis.
 *
 * A C++ Chassis object only needs to be created if the physical chassis
 * contains regulators that need to be configured or monitored.
 */
class Chassis
{
  public:
    // Specify which compiler-generated methods we want
    Chassis() = delete;
    Chassis(const Chassis&) = delete;
    Chassis(Chassis&&) = delete;
    Chassis& operator=(const Chassis&) = delete;
    Chassis& operator=(Chassis&&) = delete;
    ~Chassis() = default;

    /**
     * Constructor.
     *
     * Throws an exception if any of the input parameters are invalid.
     *
     * @param number Chassis number within the system.  Chassis numbers start at
     *               1 because chassis 0 represents the entire system.
     * @param devices Devices within this chassis, if any.  The vector should
     *                contain regulator devices and any related devices required
     *                to perform regulator operations.
     */
    explicit Chassis(unsigned int number,
                     std::vector<std::unique_ptr<Device>> devices =
                         std::vector<std::unique_ptr<Device>>{}) :
        number{number},
        devices{std::move(devices)}
    {
        if (number < 1)
        {
            throw std::invalid_argument{"Invalid chassis number: " +
                                        std::to_string(number)};
        }
    }

    /**
     * Returns the devices within this chassis, if any.
     *
     * The vector contains regulator devices and any related devices
     * required to perform regulator operations.
     *
     * @return devices in chassis
     */
    const std::vector<std::unique_ptr<Device>>& getDevices() const
    {
        return devices;
    }

    /**
     * Returns the chassis number within the system.
     *
     * @return chassis number
     */
    unsigned int getNumber() const
    {
        return number;
    }

  private:
    /**
     * Chassis number within the system.
     *
     * Chassis numbers start at 1 because chassis 0 represents the entire
     * system.
     */
    const unsigned int number{};

    /**
     * Devices within this chassis, if any.
     *
     * The vector contains regulator devices and any related devices
     * required to perform regulator operations.
     */
    std::vector<std::unique_ptr<Device>> devices{};
};

} // namespace phosphor::power::regulators