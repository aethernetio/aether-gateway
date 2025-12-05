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

#include "sim-gateway/gw-sim-data-bus.h"

#include <limits>
#include <cassert>
#include <algorithm>

namespace ae::gw::sim {
void GwSimDataBus::PublishDeviceData(DeviceId from_device,
                                     DataBuffer const& data) {
  for (auto const& gw : gateway_listeners_) {
    gw->PushData(from_device, data);
  }
}

void GwSimDataBus::PublishGwData(DeviceId to_device, DataBuffer const& data) {
  auto it = device_listeners_.find(to_device);
  if (it == std::end(device_listeners_)) {
    return;
  }
  it->second->PushData(data);
}

DeviceId GwSimDataBus::RegDeviceListener(DeviceListener* listener) {
  auto id = GetDeviceId();
  device_listeners_.emplace(id, listener);
  return id;
}

void GwSimDataBus::RemoveDeviceListener(DeviceId device_id) {
  device_listeners_.erase(device_id);
}

void GwSimDataBus::RegGatewayListener(GatewayListener* listener) {
  gateway_listeners_.push_back(listener);
}

void GwSimDataBus::RemoveGatewayListener(GatewayListener* listener) {
  auto it =
      std::find(gateway_listeners_.begin(), gateway_listeners_.end(), listener);
  if (it != gateway_listeners_.end()) {
    gateway_listeners_.erase(it);
  }
}

DeviceId GwSimDataBus::GetDeviceId() {
  auto id = next_device_id_++;
  assert((id < std::numeric_limits<DeviceId>::max()) && "Device ID overflow");
  return id;
}

}  // namespace ae::gw::sim
