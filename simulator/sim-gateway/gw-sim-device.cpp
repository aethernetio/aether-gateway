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

#include "sim-gateway/gw-sim-device.h"

#include "aether/tele/tele.h"

namespace ae::gw::sim {
namespace gw_sim_device_internal {
class GwStreamWriteAction final : public StreamWriteAction {
 public:
  explicit GwStreamWriteAction(ActionContext action_context)
      : StreamWriteAction{action_context} {
    state_ = State::kDone;
  }
};
}  // namespace gw_sim_device_internal

GwSimDevice::GwSimDevice(ActionContext action_context,
                         GwSimDataBus& gw_sim_data_bus)
    : action_context_{action_context},
      gw_sim_data_bus_{&gw_sim_data_bus},
      device_id_{gw_sim_data_bus_->RegDeviceListener(this)},
      gateway_api_{protocol_context_},
      gateway_client_api_{protocol_context_} {}

GwSimDevice::~GwSimDevice() {
  gw_sim_data_bus_->RemoveDeviceListener(device_id_);
}

ActionPtr<StreamWriteAction> GwSimDevice::ToServer(ClientId client_id,
                                                   ServerId server_id,
                                                   DataBuffer&& data) {
  auto api = ApiContext{gateway_api_};
  api->to_server_id(client_id, server_id, std::move(data));
  DataBuffer packet = std::move(api);

  AE_TELED_DEBUG("Publish from device_id {} data {}",
                 static_cast<int>(device_id_), packet);

  gw_sim_data_bus_->PublishDeviceData(device_id_, packet);
  return ActionPtr<gw_sim_device_internal::GwStreamWriteAction>{
      action_context_};
}

ActionPtr<StreamWriteAction> GwSimDevice::ToServer(
    ClientId client_id, ServerEndpoints const& server_endpoints,
    DataBuffer&& data) {
  auto api = ApiContext{gateway_api_};
  api->to_server(client_id, server_endpoints, std::move(data));
  DataBuffer packet = std::move(api);

  AE_TELED_DEBUG("Publish from device_id {} data {}",
                 static_cast<int>(device_id_), packet);

  gw_sim_data_bus_->PublishDeviceData(device_id_, packet);
  return ActionPtr<gw_sim_device_internal::GwStreamWriteAction>{
      action_context_};
}

GwSimDevice::FromServerEvent::Subscriber GwSimDevice::from_server_event() {
  return gateway_client_api_.from_server_event();
}

void GwSimDevice::PushData(DataBuffer const& data) {
  AE_TELED_DEBUG("Push data from gateway {}", data);
  auto parser = ApiParser{protocol_context_, data};
  parser.Parse(gateway_client_api_);
}
}  // namespace ae::gw::sim
