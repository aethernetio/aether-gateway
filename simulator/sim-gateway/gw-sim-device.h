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

#ifndef SIM_GATEWAY_GW_SIM_DEVICE_H_
#define SIM_GATEWAY_GW_SIM_DEVICE_H_

#include <cstdint>

#include "sim-gateway/sim-gateway-config.h"

#include "aether/types/server_id.h"
#include "aether/types/client_id.h"
#include "aether/gateway_api/gateway_api.h"
#include "aether/work_cloud_api/server_descriptor.h"

#include "sim-gateway/gw-sim-data-bus.h"

namespace ae::gw::sim {
class GwSimDevice final : public DeviceListener {
 public:
  using FromServerIdEvent =
      typename decltype(std::declval<GatewayClientApi>()
                            .from_server_id_event())::EventType;
  using FromServerEvent =
      typename decltype(std::declval<GatewayClientApi>()
                            .from_server_event())::EventType;

  explicit GwSimDevice(GwSimDataBus& gw_sim_data_bus);
  ~GwSimDevice() override;

  /**
   * \brief Publish data from client to server.
   */
  void PublishData(ClientId client_id, ServerId server_id,
                   DataBuffer const& data);
  void PublishData(ClientId client_id, ServerEndpoints const& server_endpoints,
                   DataBuffer const& data);

  void PushData(DataBuffer const& data) override;

  FromServerIdEvent::Subscriber from_server_id_event();
  FromServerEvent::Subscriber from_server_event();

 private:
  GwSimDataBus* gw_sim_data_bus_;
  DeviceId device_id_;

  ProtocolContext protocol_context_;
  GatewayApi gateway_api_;
  GatewayClientApi gateway_client_api_;
};
}  // namespace ae::gw::sim

#endif  // SIM_GATEWAY_GW_SIM_DEVICE_H_
