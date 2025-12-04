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

project(sim-alice-bob)

list(APPEND sim_alice_bob_srcs
    sim-alice-bob.cpp
)

add_executable(sim-alice-bob ${sim_alice_bob_srcs})

target_include_directories(sim-alice-bob PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(sim-alice-bob PRIVATE sim-gateway aether-gateway)

add_test(NAME sim-alice-bob COMMAND $<TARGET_FILE:sim-alice-bob>)
