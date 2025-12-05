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

#include "sim-gateway/gw-sim-adapter.h"

namespace ae::gw::sim {
GwSimAdapter::GwSimAdapter(GwSimDataBus& data_bus, Aether::ptr aether,
                           Domain* domain)
    : Adapter{domain}, data_bus_{&data_bus} {
  access_point_ = domain->CreateObj<GwSimAccessPoint>(
      std::move(aether), Adapter::ptr{MakePtrFromThis(this)});
}

std::vector<AccessPoint::ptr> GwSimAdapter::access_points() {
  return {access_point_};
}

GwSimDataBus& GwSimAdapter::data_bus() { return *data_bus_; }

}  // namespace ae::gw::sim
