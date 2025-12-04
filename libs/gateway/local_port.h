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

#ifndef GATEWAY_LOCAL_PORT_H_
#define GATEWAY_LOCAL_PORT_H_

#include <map>
#include <variant>

#include "aether/all.h"

#include "gateway/gw_stream.h"
#include "gateway/api/client_api.h"

namespace ae::gw {
class Gateway;
class LocalPort {
  friend class GatewayApiImpl;

 public:
  using ServeKind = std::variant<ServerId, ServerEndpoints>;

  struct Key {
    Key() = default;
    Key(std::uint8_t did, ClientId cid, ServeKind const& server);

    std::uint8_t device_id;
    ClientId client_id;
    std::uint32_t server_identity;
  };

  struct StreamStore {
    TimePoint last_used;
    ServeKind server;
    std::unique_ptr<GwStream> stream;
  };

  using Output = Event<void(std::uint8_t device_id, DataBuffer const& data)>;

  explicit LocalPort(Gateway& gateway);

  /**
   * \brief Input data from local device
   */
  void Input(std::uint8_t device_id, DataBuffer const& data);

  /**
   * \brief Output data event to local device
   */
  Output::Subscriber output_event();

 private:
  ByteIStream& OpenStream(std::uint8_t device_id, ClientId client_id,
                          ServerId server_id);
  ByteIStream& OpenStream(std::uint8_t device_id, ClientId client_id,
                          ServerEndpoints const& server_endpoints);
  ByteIStream& OpenStream(Key const& key, ServeKind const& server);

  void OutData(Key const& key, DataBuffer const& data);
  void StreamState(Key const& key);

  Gateway* gateway_;
  ProtocolContext protocol_context_;
  Output output_event_;
  ClientApi client_api_;

  std::map<Key, StreamStore> stream_store_;
  MultiSubscription out_data_subs_;
  MultiSubscription update_stream_subs_;
};
}  // namespace ae::gw

#endif  // GATEWAY_LOCAL_PORT_H_
