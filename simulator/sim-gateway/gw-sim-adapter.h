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

#ifndef SIM_GATEWAY_GW_SIM_ADAPTER_H_
#define SIM_GATEWAY_GW_SIM_ADAPTER_H_

#include "aether/aether.h"
#include "aether/adapters/adapter.h"

#include "sim-gateway/gw-sim-data-bus.h"
#include "sim-gateway/gw-sim-access-point.h"

namespace ae::gw::sim {
class GwSimAdapter final : public Adapter {
  AE_OBJECT(GwSimAdapter, Adapter, 0)
  GwSimAdapter() = default;

 public:
  GwSimAdapter(GwSimDataBus& data_bus, Aether::ptr aether, Domain* domain);

  AE_OBJECT_REFLECT(AE_MMBRS(access_point_))

  std::vector<AccessPoint::ptr> access_points() override;

  GwSimDataBus& data_bus();

 private:
  GwSimAccessPoint::ptr access_point_;
  GwSimDataBus* data_bus_;
};
}  // namespace ae::gw::sim

#endif  // SIM_GATEWAY_GW_SIM_ADAPTER_H_
