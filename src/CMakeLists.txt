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
            "main.cpp"
            "gateway_server.cpp"
            "gateway/server_stream.cpp"
            "gateway/server_stream_manager.cpp"
            "gateway/gw_stream.cpp"
            "gateway/gateway.cpp"
            "gateway/local_port.cpp"
            "gateway/api/client_api.cpp"
)

list(APPEND lora_gateways_srcs
            "lora_gateways/dx_smart_lr02_gw.cpp"
            "lora_gateways/lora_gateway_factory.cpp")

if (NOT CM_PLATFORM)
  project("aether-gateway-app" VERSION "1.0.0" LANGUAGES C CXX)

  add_subdirectory("../libs/aether/aether" aether)

  add_executable(${PROJECT_NAME} ${gateway_srcs}
                                 ${lora_gateways_srcs})

  target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${PROJECT_NAME} PRIVATE aether)

  include(GNUInstallDirs)
  install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
else()
  idf_component_register(SRCS
      ${gateway_srcs}
      ${lora_gateways_srcs}
      INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
      PRIV_REQUIRES aether)
endif()
