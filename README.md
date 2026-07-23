# Æthernet Gateway

An experimental C++ gateway for connecting non-IP devices to Æthernet. The project combines the Æthernet C++ SDK with gateway-side protocol bridging and can be built for desktop systems or ESP32 with ESP-IDF.

[Æthernet C++ SDK](https://github.com/aethernetio/aether-client-cpp) · [Documentation](https://aethernet.io/documentation)

## Status

This repository is under active development. Protocol compatibility, configuration, and hardware support may change before a stable release.

## Requirements

- Git;
- CMake 3.16 or newer;
- a C++17 compiler for desktop builds;
- ESP-IDF for ESP32 builds.

## Clone

```bash
git clone --recurse-submodules https://github.com/aethernetio/aether-gateway.git
cd aether-gateway
```

For an existing clone:

```bash
git submodule update --init --recursive
```

## Desktop build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

AddressSanitizer can be enabled for compatible desktop toolchains:

```bash
cmake -S . -B build -DAE_ADDRESS_SANITIZE=ON
```

## ESP32 build

Configure the ESP-IDF environment, select the target required by your hardware, and build from the repository root:

```bash
idf.py -B build set-target esp32
idf.py -B build build
```

Hardware-specific radio and gateway configuration is not yet documented as a stable public interface.

## Use cases

- bridging constrained or non-IP sensor networks to Æthernet;
- testing gateway-based connectivity for LoRa or custom transports;
- running the Æthernet client on a gateway rather than on every endpoint.

## License

Apache License 2.0. See [LICENSE](LICENSE).
