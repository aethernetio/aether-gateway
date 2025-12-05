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

#include "sim-gateway/gw-sim-transport.h"

namespace ae::gw::sim {
namespace gw_sim_transport_internal {
class GwStreamWriteAction final : public StreamWriteAction {
 public:
  explicit GwStreamWriteAction(ActionContext action_context)
      : StreamWriteAction{action_context} {
    state_ = State::kDone;
  }
};
}  // namespace gw_sim_transport_internal

ClientId GwSimTransport::next_client_id_ = 0;

GwSimTransport::GwSimTransport(ActionContext action_context, ServerId server_id,
                               GwSimDevice& gw_device)
    : GwSimTransport{action_context, gw_device, server_id} {}

GwSimTransport::GwSimTransport(ActionContext action_context,
                               ServerEndpoints server_endpoints,
                               GwSimDevice& gw_device)
    : GwSimTransport{action_context, gw_device, std::move(server_endpoints)} {}

GwSimTransport::GwSimTransport(ActionContext action_context,
                               GwSimDevice& gw_device,
                               std::variant<ServerId, ServerEndpoints> server)
    : action_context_{action_context},
      client_id_{next_client_id_++},
      gw_device_{&gw_device},
      stream_info_{},
      server_{std::move(server)} {
  stream_info_.is_reliable = false;
  stream_info_.is_writable = true;
  stream_info_.link_state = LinkState::kLinked;

  from_server_sub_ = std::visit(
      reflect::OverrideFunc{
          [this](ServerId server_id) -> Subscription {
            return gw_device_->from_server_id_event().Subscribe(
                [this, server_id](ClientId cid, ServerId sid,
                                  DataBuffer const data) {
                  if (client_id_ != cid || server_id != sid) {
                    return;
                  }
                  out_data_event_.Emit(data);
                });
          },
          [this](ServerEndpoints const& server_endpoints) -> Subscription {
            auto server_hash = static_cast<std::uint32_t>(
                std::hash<ServerEndpoints>()(server_endpoints));
            return gw_device_->from_server_event().Subscribe(
                [this, server_hash](ClientId cid, std::uint32_t sh,
                                    DataBuffer const& data) {
                  if (client_id_ != cid || server_hash != sh) {
                    return;
                  }
                  out_data_event_.Emit(data);
                });
          }},
      server_);
}

ActionPtr<StreamWriteAction> GwSimTransport::Write(DataBuffer&& in_data) {
  std::visit(
      [&](auto const& server) {
        return gw_device_->PublishData(client_id_, server, in_data);
      },
      server_);

  return ActionPtr<gw_sim_transport_internal::GwStreamWriteAction>{
      action_context_};
}

GwSimTransport::StreamUpdateEvent::Subscriber
GwSimTransport::stream_update_event() {
  return EventSubscriber{stream_update_event_};
}

StreamInfo GwSimTransport::stream_info() const { return stream_info_; }

GwSimTransport::OutDataEvent::Subscriber GwSimTransport::out_data_event() {
  return EventSubscriber{out_data_event_};
}

void GwSimTransport::Restream() { /* nothing */ }

}  // namespace ae::gw::sim
