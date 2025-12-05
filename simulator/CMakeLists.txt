# Copyright 2024 Aethernet Inc.
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

cmake_minimum_required( VERSION 3.16 )

project(simulator)

enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/run)

set(USER_CONFIG "${CMAKE_CURRENT_LIST_DIR}/user_config.h" CACHE STRING "" FORCE)

add_subdirectory(../libs/aether/aether aether)
add_subdirectory(../libs/gateway gateway)
add_subdirectory(./sim-gateway sim-gateway)

add_subdirectory(./tests/sim-alice-bob)
