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

#ifndef AETHER_CONSTRUCT_LORA_GATEWAY_H_
#define AETHER_CONSTRUCT_LORA_GATEWAY_H_

#include "aether_construct.h"
#include "lora_gateways/lora_gateway_driver_types.h"

#if CLOUD_TEST_LORA_GATEWAY

namespace ae::gateway_server {
static constexpr std::string_view kSerialPortLoraGateway =
    "COM1";  // Lora gateway serial port
SerialInit serial_init_lora_gateway = {std::string(kSerialPortLoraGateway),
                                       kBaudRate::kBaudRate9600};

ae::LoraGatewayPowerSaveParam psp{
    {kLoraGatewayMode::kTransparentTransmission},  // kLoraGatewayMode
    {kLoraGatewayLevel::kLevel0},                  // kLoraGatewayLevel
    {kLoraGatewayPower::kPower22},                 // kLoraGatewayPower
    {kLoraGatewayBandWidth::kBandWidth125K},       // kLoraGatewayBandWidth
    {kLoraGatewayCodingRate::kCR4_6},              // kLoraGatewayCodingRate
    {kLoraGatewaySpreadingFactor::kSF12}  // kLoraGatewaySpreadingFactor
};

ae::LoraGatewayInit const lora_gateway_init{
    serial_init_lora_gateway,                  // Serial port
    psp,                                       // Power Save Parameters
    {kLoraModuleFreqRange::kFREUndef},         // Freq range
    {0},                                       // Lora gateway my address
    {0},                                       // Channel
    {kLoraGatewayCRCCheck::kCRCOff},           // CRC check
    {kLoraGatewayIQSignalInversion::kIQoff}};  // Signal inversion

static RcPtr<AetherApp> construct_aether_app() {
  return AetherApp::Construct(
      AetherAppContext()
#  if defined AE_DISTILLATION
          .AdaptersFactory([](AetherAppContext const& context) {
            auto adapter_registry =
                context.domain().CreateObj<AdapterRegistry>();
            adapter_registry->Add(context.domain().CreateObj<EthernetAdapter>(
                GlobalId::kEthernetAdapter, context.aether(), context.poller(),
                context.dns_resolver()));
            return adapter_registry;
          })
#  endif
  );
}
}  // namespace ae::gateway_server

#endif
#endif  // AETHER_CONSTRUCT_LORA_GATEWAY_H_
