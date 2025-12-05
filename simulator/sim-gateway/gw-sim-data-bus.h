/*
 * Copyright 2025 Aethernet Inc.
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

#ifndef SIM_GATEWAY_GW_SIM_DATA_BUS_H_
#define SIM_GATEWAY_GW_SIM_DATA_BUS_H_

#include <map>
#include <cstdint>

#include "aether/types/data_buffer.h"

namespace ae::gw::sim {
using DeviceId = std::uint8_t;

class DeviceListener {
 public:
  virtual ~DeviceListener() = default;

  virtual void PushData(DataBuffer const& data) = 0;
};

class GatewayListener {
 public:
  virtual ~GatewayListener() = default;

  virtual void PushData(DeviceId device_id, DataBuffer const& data) = 0;
};

/**
 * \brief Data bus for simulator gateway.
 * It allows to publish data from/to device and receive published data by
 * listeners.
 */
class GwSimDataBus {
 public:
  void PublishDeviceData(DeviceId from_device, DataBuffer const& data);
  void PublishGwData(DeviceId to_device, DataBuffer const& data);

  DeviceId RegDeviceListener(DeviceListener* listener);
  void RemoveDeviceListener(DeviceId device_id);
  void RegGatewayListener(GatewayListener* listener);
  void RemoveGatewayListener(GatewayListener* listener);

 private:
  DeviceId GetDeviceId();

  DeviceId next_device_id_ = 1;
  std::map<DeviceId, DeviceListener*> device_listeners_;
  std::vector<GatewayListener*> gateway_listeners_;
};
}  // namespace ae::gw::sim

#endif  // SIM_GATEWAY_GW_SIM_DATA_BUS_H_
