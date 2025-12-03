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

#ifndef SIM_GATEWAY_GW_SIM_DEVICE_PORT_H_
#define SIM_GATEWAY_GW_SIM_DEVICE_PORT_H_

#include "gateway/local_port.h"

#include "sim-gateway/gw-sim-data-bus.h"

namespace ae::gw::sim {
class GwSimDevicePort final : public GatewayListener {
 public:
  GwSimDevicePort(GwSimDataBus& gw_sim_data_bus, LocalPort& local_port);
  ~GwSimDevicePort() override;

  void PushData(DeviceId device_id, DataBuffer const& data) override;

 private:
  GwSimDataBus* gw_sim_data_bus_;
  LocalPort* local_port_;
  Subscription output_sub_;
};
}  // namespace ae::gw::sim

#endif  // SIM_GATEWAY_GW_SIM_DEVICE_PORT_H_
