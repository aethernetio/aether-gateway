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

#ifndef SIM_GATEWAY_GW_SIM_ACCESS_POINT_H_
#define SIM_GATEWAY_GW_SIM_ACCESS_POINT_H_

#include "aether/aether.h"
#include "aether/server.h"
#include "aether/adapters/adapter.h"
#include "aether/access_points/access_point.h"

#include "sim-gateway/gw-sim-device.h"

namespace ae::gw::sim {
class GwSimAccessPoint final : public AccessPoint {
  AE_OBJECT(GwSimAccessPoint, AccessPoint, 0)
  GwSimAccessPoint() = default;

 public:
  class JoinAction : public Action<JoinAction> {
   public:
    explicit JoinAction(ActionContext action_context);
    UpdateStatus Update();
  };

  GwSimAccessPoint(Aether::ptr aether, Adapter::ptr adapter, Domain* domain);

  AE_OBJECT_REFLECT(AE_MMBRS(adapter_))

  ActionPtr<JoinAction> Join();

  std::vector<ObjPtr<Channel>> GenerateChannels(
      Server::ptr const& server) override;

  Aether::ptr const& aether() const;
  GwSimDevice& gw_device();

 private:
  Aether::ptr aether_;
  Adapter::ptr adapter_;
  std::unique_ptr<GwSimDevice> gw_device_;
};
}  // namespace ae::gw::sim

#endif  // SIM_GATEWAY_GW_SIM_ACCESS_POINT_H_
