# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

r2web3 is a C++ Web3 library for Arduino microcontrollers (ESP8266, ESP32, Atmel SAM) that provides Ethereum-compatible blockchain interaction via JSON-RPC. It also supports desktop Linux builds using libcurl.

## Build System

Uses **PlatformIO** with the Arduino framework. No Makefile or CMake.

- **PlatformIO build**: `pio run` (or use Arduino IDE with the library installed in `~/Arduino/libraries/`)
- **No formal test suite yet** — testing is manual via the example sketch
- Include paths: `src`, `src/Shared`, `src/Blockchain`, `src/r2Common/lib/r2common/src`, `src/r2Common/lib/r2common/src/Network`

## Architecture

All public APIs are in the `blockchain::` namespace.

### Modules

- **Blockchain** (`src/Blockchain/`): Core chain interaction — `Chain` is the main entry point for RPC calls (send transactions, view calls, balances, gas estimation). `Account` derives addresses from private keys via secp256k1. `EthereumTransaction` + `EthereumSigner` + `EthereumTransactionFactory` handle transaction creation, signing, and RLP serialization. `ContractCall` + `ABIEncoder` handle smart contract method encoding.
- **r2Common** (`src/r2Common/`, git subtree from `r2common` repo): Self-contained `r2common::` namespace. `Result<T>` error monad, `NetworkFacade` interface with `ESPNetwork` (Arduino) and `CurlNetwork` (desktop) implementations, HTTP request/response types. The r2common repo is a PlatformIO project; the subtree imports the whole repo at `src/r2Common/`, but library source files live at `src/r2Common/lib/r2common/src/`. The placeholder `src/r2Common/src/main.cpp` is excluded from compilation via srcFilter. Managed via `git subtree` — pull updates with `git subtree pull --prefix=src/r2Common r2common main --squash`, push changes back with `git subtree push --prefix=src/r2Common r2common main`. Includes use library-style paths (e.g. `#include "R2Logger.h"`, `#include "Network/NetworkFacade.h"`) resolved via build flags in `library.json`.
- **Shared** (`src/Shared/`): `Common.h` has byte/string utilities with functional pipe operators (`|`). `BigNumber` for arbitrary-precision blockchain values. `R2Web3Log` singleton logger.
- **Cryptography** (`src/cryptography/`): Bundled crypto primitives (secp256k1, Keccak-256, SHA-256, ECDSA, BIP32).

### Key Design Patterns

- **Template factories**: `TransactionFactory<Properties, Transaction>` and `Signer<T>` are abstract templates with Ethereum-specific implementations
- **Result monad**: `Result<T>` used throughout for Arduino-friendly error handling (no exceptions on microcontrollers)
- **Pipe operators**: Custom `operator|` in `byte_array`/`char_string` namespaces for functional data transformation chaining
- **`ENC()` macro**: Convenience macro in `Contract.h` for building typed contract call parameters

### Error Handling

- Desktop: C++ exceptions via `THROW()` macro
- Arduino: Custom error handler `_thorException_arduino` (no exceptions on MCU). Suppressed entirely when `R2WEB3_LOGGING_DISABLED` is defined in `src/configuration.h`

## Platform Conditional Compilation

The codebase uses `#ifdef ARDUINO` extensively. When modifying network or logging code, ensure both Arduino and desktop code paths compile. The main header `src/r2web3.h` conditionally includes ESP vs curl network implementations.
