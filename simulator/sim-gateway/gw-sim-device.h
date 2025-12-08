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

#include "aether/types/server_id.h"
#include "aether/types/client_id.h"
#include "aether/gateway_api/gateway_api.h"
#include "aether/transport/gateway/gateway_device.h"

#include "sim-gateway/gw-sim-data-bus.h"

namespace ae::gw::sim {
class GwSimDevice final : public IGatewayDevice, public DeviceListener {
 public:
  explicit GwSimDevice(ActionContext action_context,
                       GwSimDataBus& gw_sim_data_bus);
  ~GwSimDevice() override;

  ActionPtr<StreamWriteAction> ToServer(ClientId client_id, ServerId server_id,
                                        DataBuffer&& data) override;

  ActionPtr<StreamWriteAction> ToServer(ClientId client_id,
                                        ServerEndpoints const& server_endpoints,
                                        DataBuffer&& data) override;

  FromServerEvent::Subscriber from_server_event() override;

  void PushData(DataBuffer const& data) override;

 private:
  ActionContext action_context_;
  GwSimDataBus* gw_sim_data_bus_;
  DeviceId device_id_;

  ProtocolContext protocol_context_;
  GatewayApi gateway_api_;
  GatewayClientApi gateway_client_api_;
};
}  // namespace ae::gw::sim

#endif  // SIM_GATEWAY_GW_SIM_DEVICE_H_
