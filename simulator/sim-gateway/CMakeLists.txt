# Copyright 2025 Aethernet Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.16.0)

list(APPEND sim_gateway_srcs
  "gw-sim-access-point.cpp"
  "gw-sim-adapter.cpp"
  "gw-sim-channel.cpp"
  "gw-sim-data-bus.cpp"
  "gw-sim-device.cpp"
  "gw-sim-transport.cpp"

  "gw-sim-device-port.cpp"
)

project("sim-gateway" VERSION "1.0.0" LANGUAGES C CXX)

add_library(${PROJECT_NAME} STATIC ${sim_gateway_srcs})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(${PROJECT_NAME} PUBLIC aether aether-gateway)
