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

#include "gateway/server_stream_manager.h"

#include <utility>
#include <cassert>
#include <cstdint>

#include "gateway/gateway.h"
#include "gateway/server_stream.h"

namespace ae::gw {
namespace server_stream_manager_internal {
class ExistingStreamGetAction : public StreamGetAction {
 public:
  ExistingStreamGetAction(ActionContext action_context,
                          std::shared_ptr<ByteIStream> stream)
      : StreamGetAction{action_context}, stream_{std::move(stream)} {}

  UpdateStatus Update() override { return UpdateStatus::Result(); }

  std::shared_ptr<ByteIStream> const& stream() const override {
    return stream_;
  }

 private:
  std::shared_ptr<ByteIStream> stream_;
};

class RequestServerStreamGetAction : public StreamGetAction {
  enum class State : std::uint8_t {
    kCheckServerExists,
    kRequestServer,
    kResult,
    kError,
  };

 public:
  RequestServerStreamGetAction(ActionContext& action_context,
                               ServerStreamManager& server_stream_manager,
                               ServerId server_id)
      : StreamGetAction{action_context},
        server_stream_manager_{&server_stream_manager},
        server_id_{server_id},
        state_{State::kCheckServerExists} {
    state_.changed_event().Subscribe([this](auto) { Action::Trigger(); });
  }

  UpdateStatus Update() override {
    if (state_.changed()) {
      switch (state_.Acquire()) {
        case State::kCheckServerExists:
          CheckServerExists();
          break;
        case State::kRequestServer:
          RequestServer();
          break;
        case State::kResult:
          return UpdateStatus::Result();
        case State::kError:
          return UpdateStatus::Error();
      }
    }

    return {};
  }

  std::shared_ptr<ByteIStream> const& stream() const override {
    return stream_;
  }

 private:
  void CheckServerExists() {
    auto const& aether = server_stream_manager_->gateway_->aether;
    auto server = aether->GetServer(server_id_);
    if (server) {
      stream_ = server_stream_manager_->MakeStream(std::move(server));
      server_stream_manager_->CacheStream(server_id_, stream_);
      state_ = State::kResult;
    } else {
      state_ = State::kRequestServer;
    }
  }

  void RequestServer() {
    auto const& aether = server_stream_manager_->gateway_->aether;
    auto const& client = server_stream_manager_->gateway_->gateway_client;

    get_servers_action_ = OwnActionPtr<GetServersAction>{
        *aether, std::vector{server_id_}, client->cloud_connection(),
        RequestPolicy::MainServer{}};

    get_servers_action_->StatusEvent().Subscribe(ActionHandler{
        OnResult{[this](auto const& action) {
          auto const& servers = action.servers();
          assert(!servers.empty() && "Servers should not be empty on result");

          auto const& sd = servers.front();
          std::vector<UnifiedAddress> endpoints;
          for (auto const& ipp : sd.ips) {
            for (auto const& proto_port : ipp.protocol_and_ports) {
              endpoints.emplace_back(IpAddressPortProtocol{
                  {ipp.ip, proto_port.port}, proto_port.protocol});
            }
          }

          auto server = server_stream_manager_->BuildServer(
              sd.server_id, {std::move(endpoints)});
          stream_ = server_stream_manager_->MakeStream(std::move(server));
          server_stream_manager_->CacheStream(server_id_, stream_);
          state_ = State::kResult;
        }},
        OnError{[this]() { state_ = State::kError; }},
    });
  }

  ServerStreamManager* server_stream_manager_;
  ServerId server_id_;
  StateMachine<State> state_;
  OwnActionPtr<GetServersAction> get_servers_action_;

  std::shared_ptr<ByteIStream> stream_;
};

}  // namespace server_stream_manager_internal

ServerStreamManager::ServerStreamManager(Gateway& gateway)
    : gateway_{&gateway} {}

ActionPtr<StreamGetAction> ServerStreamManager::GetStream(ServerId server_id,
                                                          bool cache) {
  if (cache) {
    auto& stream_cache = OpenCache();
    auto it = stream_cache.find(server_id);
    if (it != std::end(stream_cache)) {
      return ActionPtr<server_stream_manager_internal::ExistingStreamGetAction>{
          *gateway_, it->second.lock()};
    }
  }
  return ActionPtr<
      server_stream_manager_internal::RequestServerStreamGetAction>{
      *gateway_, *this, server_id};
}

ActionPtr<StreamGetAction> ServerStreamManager::GetStream(
    ServerEndpoints const& endpoints) {
  auto server = BuildServer(0, endpoints);
  auto stream = MakeStream(std::move(server));

  return ActionPtr<server_stream_manager_internal::ExistingStreamGetAction>{
      *gateway_, std::move(stream)};
}

Server::ptr ServerStreamManager::BuildServer(ServerId server_id,
                                             ServerEndpoints const& endpoints) {
  auto const& aether = gateway_->aether;

  if (server_id != 0) {
    auto cached = aether->GetServer(server_id);
    if (cached) {
      return cached;
    }
  }

  auto server =
      aether->domain_->CreateObj<Server>(server_id, endpoints.endpoints);
  server->Register(aether->adapter_registry);
  if (server_id != 0) {
    aether->AddServer(server);
  }
  return server;
}

std::shared_ptr<ByteIStream> ServerStreamManager::MakeStream(
    Server::ptr server) {
  assert(server && "Server should not be null");

  auto stream = std::make_shared<ServerStream>(*gateway_, std::move(server));
  return stream;
}

void ServerStreamManager::CacheStream(
    ServerId server_id, std::shared_ptr<ByteIStream> const& stream) {
  stream_cache_.insert({server_id, stream});
}

std::map<ServerId, std::weak_ptr<ByteIStream>>&
ServerStreamManager::OpenCache() {
  for (auto it = stream_cache_.begin(); it != stream_cache_.end();) {
    if (it->second.expired()) {
      it = stream_cache_.erase(it);
    } else {
      ++it;
    }
  }
  return stream_cache_;
}

}  // namespace ae::gw
