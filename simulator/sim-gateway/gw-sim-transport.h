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

#ifndef SIM_GATEWAY_GW_SIM_TRANSPORT_H_
#define SIM_GATEWAY_GW_SIM_TRANSPORT_H_

#ifndef AE_SUPPORT_GATEWAY
#  define AE_SUPPORT_GATEWAY 1
#endif

#include "aether/types/client_id.h"
#include "aether/stream_api/istream.h"

#include "sim-gateway/gw-sim-device.h"

namespace ae::gw::sim {
class GwSimTransport final : public ByteIStream {
 public:
  static ClientId next_client_id_;

  GwSimTransport(ActionContext action_context, ServerId server_id,
                 GwSimDevice& gw_device);
  GwSimTransport(ActionContext action_context, ServerEndpoints server_endpoints,
                 GwSimDevice& gw_device);

  ActionPtr<StreamWriteAction> Write(DataBuffer&& in_data) override;
  StreamUpdateEvent::Subscriber stream_update_event() override;
  StreamInfo stream_info() const override;
  OutDataEvent::Subscriber out_data_event() override;
  void Restream() override;

 private:
  GwSimTransport(ActionContext action_context, GwSimDevice& gw_device,
                 std::variant<ServerId, ServerEndpoints> server);

  ActionContext action_context_;
  ClientId client_id_;
  GwSimDevice* gw_device_;
  StreamInfo stream_info_;
  StreamUpdateEvent stream_update_event_;
  OutDataEvent out_data_event_;
  Subscription from_server_sub_;
  std::variant<ServerId, ServerEndpoints> server_;
};
}  // namespace ae::gw::sim

#endif  // SIM_GATEWAY_GW_SIM_TRANSPORT_H_
