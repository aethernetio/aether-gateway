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

#ifndef SIM_GATEWAY_GW_SIM_CHANNEL_H_
#define SIM_GATEWAY_GW_SIM_CHANNEL_H_

#include "aether/server.h"
#include "aether/channels/channel.h"

#include "sim-gateway/gw-sim-access-point.h"

namespace ae::gw::sim {
class GwSimChannel final : public Channel {
  AE_OBJECT(GwSimChannel, Channel, 0)
  GwSimChannel() = default;

 public:
  GwSimChannel(GwSimAccessPoint::ptr access_point, Server::ptr server,
               Domain* domain);

  AE_OBJECT_REFLECT(AE_MMBRS(access_point_, server_))

  ActionPtr<TransportBuilderAction> TransportBuilder() override;

 private:
  GwSimAccessPoint::ptr access_point_;
  Server::ptr server_;
};
}  // namespace ae::gw::sim

#endif  // SIM_GATEWAY_GW_SIM_CHANNEL_H_
