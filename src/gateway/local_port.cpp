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

#include "gateway/local_port.h"

#include <tuple>
#include <utility>

#include "gateway/gateway.h"
#include "gateway/api/gateway_api.h"

namespace ae {
class GatewayApiImpl : public GatewayApi {
 public:
  explicit GatewayApiImpl(std::uint8_t device_id, LocalPort& local_port)
      : GatewayApi{local_port.protocol_context_},
        device_id_{device_id},
        local_port_{&local_port} {}

  void ToServerId(ClientId client_id, ServerId server_id,
                  DataBuffer data) override {
    // Write data to the stream
    local_port_->OpenStream(device_id_, client_id, server_id)
        .Write(std::move(data));
  }

  void ToServer(ClientId client_id, ServerEndpoints server_endpoints,
                DataBuffer data) override {
    // Write data to the stream
    local_port_->OpenStream(device_id_, client_id, server_endpoints)
        .Write(std::move(data));
  }

 private:
  std::uint8_t device_id_;
  LocalPort* local_port_;
};

LocalPort::Key::Key(std::uint8_t did, ClientId cid, ServeKind const& server)
    : device_id{did},
      client_id{cid},
      server_identity{std::visit(
          reflect::OverrideFunc{[&](ServerId const& server) {
                                  return static_cast<std::uint32_t>(server);
                                },
                                [&](ServerEndpoints const& endpoints) {
                                  return static_cast<std::uint32_t>(
                                      std::hash<ServerEndpoints>{}(endpoints));
                                }},
          server)} {}

bool operator<(LocalPort::Key const& a, LocalPort::Key const& b) {
  return std::tuple{a.device_id, a.client_id, a.server_identity} <
         std::tuple{b.device_id, b.client_id, b.server_identity};
}

LocalPort::LocalPort(Gateway& gateway)
    : gateway_{&gateway}, client_api_{protocol_context_} {}

void LocalPort::Input(std::uint8_t device_id, DataBuffer const& data) {
  auto parser = ApiParser{protocol_context_, data};
  auto api = GatewayApiImpl{device_id, *this};
  parser.Parse(api);
}

LocalPort::Output::Subscriber LocalPort::output_event() {
  return EventSubscriber{output_event_};
}

ByteIStream& LocalPort::OpenStream(std::uint8_t device_id, ClientId client_id,
                                   ServerId server_id) {
  return OpenStream(Key{device_id, client_id, server_id}, server_id);
}
ByteIStream& LocalPort::OpenStream(std::uint8_t device_id, ClientId client_id,
                                   ServerEndpoints const& server_endpoints) {
  return OpenStream(Key{device_id, client_id, server_endpoints},
                    server_endpoints);
}

ByteIStream& LocalPort::OpenStream(Key const& key, ServeKind const& server) {
  auto it = stream_store_.find(key);
  if (it == std::end(stream_store_)) {
    auto stream = std::visit(
        [this](auto const& s) {
          return std::make_unique<GwStream>(*gateway_, s);
        },
        server);

    std::tie(it, std::ignore) =
        stream_store_.emplace(key, StreamStore{{}, server, std::move(stream)});

    // subscribe stream data and updates
    out_data_subs_.Push(  // ~(^o^)~
        it->second.stream->out_data_event().Subscribe(
            [this, key](auto const& data) { OutData(key, data); }));
    update_stream_subs_.Push(  // ~(^o^)~
        it->second.stream->stream_update_event().Subscribe(
            [this, key]() { StreamState(key); }));
  }

  it->second.last_used = Now();
  return *it->second.stream;
}

void LocalPort::OutData(Key const& key, DataBuffer const& data) {
  auto it = stream_store_.find(key);
  if (it == std::end(stream_store_)) {
    return;
  }
  // update the used time
  it->second.last_used = Now();

  auto api_context = ApiContext{client_api_};
  std::visit(reflect::OverrideFunc{
                 [&](ServerId server_id) {
                   api_context->from_server_id(key.client_id, server_id, data);
                 },
                 [&](ServerEndpoints const&) {
                   api_context->from_server(key.client_id, key.server_identity,
                                            data);
                 }},
             it->second.server);

  output_event_.Emit(key.device_id, DataBuffer{std::move(api_context)});
}

void LocalPort::StreamState(Key const& key) {
  auto it = stream_store_.find(key);
  if (it == std::end(stream_store_)) {
    return;
  }
  auto const& info = it->second.stream->stream_info();
  // if link in error state, remove the stream
  if (info.link_state == LinkState::kLinkError) {
    stream_store_.erase(it);
  }
}
}  // namespace ae
