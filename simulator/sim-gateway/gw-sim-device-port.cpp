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

#include "sim-gateway/gw-sim-device-port.h"

#include "aether/tele/tele.h"

namespace ae::gw::sim {
GwSimDevicePort::GwSimDevicePort(GwSimDataBus& gw_sim_data_bus,
                                 LocalPort& local_port)
    : gw_sim_data_bus_{&gw_sim_data_bus}, local_port_{&local_port} {
  gw_sim_data_bus_->RegGatewayListener(this);
  output_sub_ = local_port_->output_event().Subscribe(
      [this](auto device_id, auto const& data) {
        AE_TELED_DEBUG("Publish data from gateway to {} {}",
                       static_cast<int>(device_id), data);
        gw_sim_data_bus_->PublishGwData(device_id, data);
      });
}

GwSimDevicePort::~GwSimDevicePort() {
  gw_sim_data_bus_->RemoveGatewayListener(this);
}

void GwSimDevicePort::PushData(DeviceId device_id, DataBuffer const& data) {
  AE_TELED_DEBUG("Push from device id {} with data {}",
                 static_cast<int>(device_id), data)
  local_port_->Input(device_id, data);
}
}  // namespace ae::gw::sim
