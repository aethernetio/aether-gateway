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

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
endif()

if (ESP_PLATFORM)
  include($ENV{IDF_PATH}/tools/cmake/project.cmake)
  set(CM_PLATFORM "ESP32")
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(AE_ADDRESS_SANITIZE Off "Enable address sanitizer")

if (CM_PLATFORM STREQUAL "ESP32")
  # ESP IDF project
  idf_build_set_property(CM_PLATFORM "${CM_PLATFORM}")
  add_compile_definitions(CM_ESP32)
  get_filename_component(configName "${CMAKE_BINARY_DIR}" NAME)

  list(APPEND EXTRA_COMPONENT_DIRS
    "./libs/aether/aether"
    "./libs/gateway"
    "./src")
  project(aether-gateway LANGUAGES CXX C ASM)
else()
  # regular cmake project
  project(aether-gateway LANGUAGES CXX C ASM)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  if (AE_ADDRESS_SANITIZE)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
  endif()

  add_subdirectory("libs/aether/aether" "aether")
  add_subdirectory("libs/gateway" "gateway")
  add_subdirectory("src" "aether-gateway")
  # add_subdirectory("tests" "tests")
endif()
