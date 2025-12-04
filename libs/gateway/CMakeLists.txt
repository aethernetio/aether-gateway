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

list(APPEND gateway_srcs
            "server_stream.cpp"
            "server_stream_manager.cpp"
            "gw_stream.cpp"
            "gateway.cpp"
            "local_port.cpp"
            "api/client_api.cpp"
)

if (NOT CM_PLATFORM)
  project("aether-gateway" VERSION "1.0.0" LANGUAGES C CXX)

  add_library(${PROJECT_NAME} STATIC ${gateway_srcs})

  target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
  target_link_libraries(${PROJECT_NAME} PUBLIC aether)
else()
  idf_component_register(SRCS
      ${gateway_srcs}
      INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/..
      REQUIRES aether)
endif()
