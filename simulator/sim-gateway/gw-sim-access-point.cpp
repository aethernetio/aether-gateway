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

#include "sim-gateway/gw-sim-access-point.h"

#include "sim-gateway/gw-sim-channel.h"
#include "sim-gateway/gw-sim-adapter.h"

namespace ae::gw::sim {
GwSimAccessPoint::JoinAction::JoinAction(ActionContext action_context)
    : Action{action_context} {}

UpdateStatus GwSimAccessPoint::JoinAction::Update() {
  return UpdateStatus::Result();
}

GwSimAccessPoint::GwSimAccessPoint(Aether::ptr aether, Adapter::ptr adapter,
                                   Domain* domain)
    : AccessPoint{domain},
      aether_{std::move(aether)},
      adapter_{std::move(adapter)} {}

ActionPtr<GwSimAccessPoint::JoinAction> GwSimAccessPoint::Join() {
  return ActionPtr<JoinAction>{*aether_};
}

std::vector<ObjPtr<Channel>> GwSimAccessPoint::GenerateChannels(
    Server::ptr const& server) {
  auto self_ptr = MakePtrFromThis(this);
  return {domain_->CreateObj<GwSimChannel>(self_ptr, server)};
}

Aether::ptr const& GwSimAccessPoint::aether() const { return aether_; }

GwSimDevice& GwSimAccessPoint::gw_device() {
  if (!gw_device_) {
    gw_device_ =
        std::make_unique<GwSimDevice>(adapter_.as<GwSimAdapter>()->data_bus());
  }
  return *gw_device_;
}

}  // namespace ae::gw::sim
