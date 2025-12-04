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

#include "gateway/gateway.h"

namespace ae::gw {
Gateway::Gateway(Aether::ptr aether, Client::ptr client, Domain* domain)
    : Obj{domain},
      aether{std::move(aether)},
      gateway_client{std::move(client)} {}

ServerStreamManager& Gateway::server_stream_manager() {
  if (!server_stream_manager_) {
    server_stream_manager_ = std::make_unique<ServerStreamManager>(*this);
  }
  return *server_stream_manager_;
}

LocalPort& Gateway::local_port() {
  if (!local_port_) {
    local_port_ = std::make_unique<LocalPort>(*this);
  }
  return *local_port_;
}

}  // namespace ae::gw
