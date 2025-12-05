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

#include "sim-gateway/gw-sim-channel.h"

#include "aether/types/state_machine.h"
#include "aether/transport/gateway/gateway_transport.h"

namespace ae::gw::sim {
namespace gw_sim_channel_internal {
class GwSimTransportBuildAction final : public TransportBuilderAction {
  enum class State : std::uint8_t {
    kJoin,
    kCreateTransport,
    kResult,
    kError,
  };

 public:
  GwSimTransportBuildAction(ActionContext action_context,
                            GwSimAccessPoint& access_point,
                            Server::ptr const& server)
      : TransportBuilderAction{action_context},
        action_context_{action_context},
        access_point_{&access_point},
        server_{server},
        state_{State::kJoin} {
    state_.changed_event().Subscribe([this](auto) { Action::Trigger(); });
  }

  UpdateStatus Update() override {
    if (state_.changed()) {
      switch (state_.Acquire()) {
        case State::kJoin:
          Join();
          break;
        case State::kCreateTransport:
          CreateTransport();
          break;
        case State::kResult:
          return UpdateStatus::Result();
        case State::kError:
          return UpdateStatus::Error();
      }
    }
    return {};
  }

  std::unique_ptr<ByteIStream> transport_stream() override {
    return std::move(transport_stream_);
  }

 private:
  void Join() {
    auto join = access_point_->Join();
    join->StatusEvent().Subscribe(ActionHandler{
        OnResult{[this]() { state_ = State::kCreateTransport; }},
        OnError{[this]() { state_ = State::kError; }},
    });
  }

  void CreateTransport() {
    auto server = server_.Lock();
    assert(server && "Server should be alive");

    if (server->server_id == 0) {
      transport_stream_ = std::make_unique<GatewayTransport>(
          ServerEndpoints{server->endpoints}, access_point_->gw_device());
    } else {
      transport_stream_ = std::make_unique<GatewayTransport>(
          server->server_id, access_point_->gw_device());
    }
    state_ = State::kResult;
  }

  ActionContext action_context_;
  GwSimAccessPoint* access_point_;
  PtrView<Server> server_;
  StateMachine<State> state_;

  std::unique_ptr<ByteIStream> transport_stream_;
};
}  // namespace gw_sim_channel_internal

GwSimChannel::GwSimChannel(GwSimAccessPoint::ptr access_point,
                           Server::ptr server, Domain* domain)
    : Channel{domain},
      access_point_{std::move(access_point)},
      server_{std::move(server)} {
  transport_properties_.connection_type = ConnectionType::kConnectionLess;
  transport_properties_.reliability = Reliability::kUnreliable;
  transport_properties_.rec_packet_size = 1024;
  transport_properties_.max_packet_size = 1024;
}

ActionPtr<TransportBuilderAction> GwSimChannel::TransportBuilder() {
  auto const& aether = access_point_->aether();
  return ActionPtr<gw_sim_channel_internal::GwSimTransportBuildAction>{
      *aether, *access_point_, server_};
}

}  // namespace ae::gw::sim
