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

#include <iostream>

#include "aether/all.h"

#include "gateway/gateway.h"
#include "gateway/local_port.h"

#include "sim-gateway/gw-sim-adapter.h"
#include "sim-gateway/gw-sim-data-bus.h"
#include "sim-gateway/gw-sim-device-port.h"

namespace ae::gw::sim {
static constexpr Uid kParentUid =
    Uid::FromString("3ac93165-3d37-4970-87a6-fa4ee27744e4");

int SimAliveBob() {
  auto gw_sim_data_bus = ae::gw::sim::GwSimDataBus{};

  // make aether gateway
  auto gateway_app = AetherApp::Construct(AetherAppContext{});
  auto select_gw_client = gateway_app->aether()->SelectClient(kParentUid, 0);

  gateway_app->WaitActions(select_gw_client);
  auto gw_client = select_gw_client->client();
  if (!gw_client) {
    return -1;
  }

  auto gateway = gateway_app->domain().CreateObj<Gateway>(
      ObjId{1337}, gateway_app->aether(), std::move(gw_client));

  // connect gateway sim data bus
  auto gw_device_port = GwSimDevicePort{gw_sim_data_bus, gateway->local_port()};

  // make aether client side
  auto client_app = AetherApp::Construct(
      AetherAppContext{}.AdaptersFactory([&](AetherAppContext const& context) {
        auto adapters = context.domain().CreateObj<AdapterRegistry>();
        adapters->Add(context.domain().CreateObj<GwSimAdapter>(
            gw_sim_data_bus, context.aether()));
        return adapters;
      }));

  int received_messages = 0;
  Client::ptr alice;
  RcPtr<P2pStream> alice_stream;
  Client::ptr bob;
  RcPtr<P2pStream> bob_stream;

  auto select_alice = client_app->aether()->SelectClient(kParentUid, 0);
  select_alice->StatusEvent().Subscribe(
      OnResult{[&](auto const& action) { alice = action.client(); }});

  auto select_bob = client_app->aether()->SelectClient(kParentUid, 1);
  select_bob->StatusEvent().Subscribe(
      OnResult{[&](auto const& action) { bob = action.client(); }});

  auto comm_event = CumulativeEvent{
      select_alice->StatusEvent(),
      select_bob->StatusEvent(),
  };

  comm_event.Subscribe([&]() {
    if (!alice || !bob) {
      return;
    }

    // send message from alice to bob and than send one back

    alice_stream = alice->message_stream_manager().CreateStream(bob->uid());
    alice_stream->out_data_event().Subscribe([&](auto const& message) {
      client_app->aether()->action_processor->get_trigger().Trigger();
      auto message_str = std::string_view(
          reinterpret_cast<char const*>(message.data()), message.size());

      std::cout << Format(">>>\n>>> Alice received message: {}\n>>>\n",
                          message_str);
      received_messages++;
    });

    std::string_view alice_message = "Hello, Bob!";
    alice_stream->Write(DataBuffer{
        reinterpret_cast<std::uint8_t const*>(alice_message.data()),
        reinterpret_cast<std::uint8_t const*>(alice_message.data() +
                                              alice_message.size())});

    bob_stream = bob->message_stream_manager().CreateStream(alice->uid());
    bob_stream->out_data_event().Subscribe([&](auto const& message) {
      client_app->aether()->action_processor->get_trigger().Trigger();
      auto message_str = std::string_view(
          reinterpret_cast<char const*>(message.data()), message.size());
      std::cout << Format(">>>\n>>> Bob received message: {}\n>>>\n",
                          message_str);
      received_messages++;

      std::string_view bob_message = "Hello, Alice!";
      bob_stream->Write(DataBuffer{
          reinterpret_cast<std::uint8_t const*>(bob_message.data()),
          reinterpret_cast<std::uint8_t const*>(bob_message.data() +
                                                bob_message.size())});
    });
  });

  // make two apps use common trigger
  auto& gateway_trigger =
      gateway_app->aether()->action_processor->get_trigger();
  auto& client_trigger = client_app->aether()->action_processor->get_trigger();
  Merge(gateway_trigger, client_trigger);

  // run common update loop
  while (!gateway_app->IsExited() && !client_app->IsExited()) {
    auto gateway_time = gateway_app->Update(Now());
    auto client_time = client_app->Update(Now());

    gateway_app->WaitUntil(std::min(gateway_time, client_time));

    if (received_messages == 2) {
      client_app->Exit(0);
    }
  }

  if (client_app->IsExited()) {
    auto code = client_app->ExitCode();
    return code != 0 ? (100 + code) : 0;
  }

  return gateway_app->ExitCode();
}
}  // namespace ae::gw::sim

int main() { return ae::gw::sim::SimAliveBob(); }
