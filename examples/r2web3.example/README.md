# r2web3 Example

A minimal example sketch showing how to use the [r2web3](../../) Ethereum/Web3 library on an
ESP32 or ESP8266. It connects to WiFi, connects to an EVM chain (ShimmerEVM testnet by default),
and demonstrates transfers, balance queries, view calls, and state-changing contract calls.

## Prerequisites

- [PlatformIO Core](https://platformio.org/install/cli) (`pio`) installed and on your `PATH`.
- An ESP32 or ESP8266 board connected via USB.

## Configure secrets

Credentials are kept out of source control. Before building, create your own `secrets.h`:

```sh
cp src/secrets_example.h src/secrets.h
```

Then edit `src/secrets.h` and fill in your values:

| Define             | Description                          |
| ------------------ | ------------------------------------ |
| `SSID`             | Your WiFi network name               |
| `WIFI_PASSWORD`    | Your WiFi password                   |
| `PRIVATE_KEY`      | The account's private key            |
| `CONTRACT_ADDRESS` | Address of a deployed contract       |

`src/secrets.h` is listed in `.gitignore`, so your credentials are never committed.

## Build & run

The project defines two PlatformIO environments in `platformio.ini`:

- `esp32dev` — ESP32 (`board = esp32dev`)
- `nodemcuv2` — ESP8266 (`board = nodemcuv2`)

Serial monitor speed is `9600` baud.

### ESP32

```sh
# Compile
pio run -e esp32dev

# Compile, upload to the board
pio run -e esp32dev -t upload

# Compile, upload, then open the serial monitor
pio run -e esp32dev -t upload -t monitor
```

### ESP8266

```sh
# Compile
pio run -e nodemcuv2

# Compile, upload to the board
pio run -e nodemcuv2 -t upload

# Compile, upload, then open the serial monitor
pio run -e nodemcuv2 -t upload -t monitor
```

### Serial monitor only

```sh
pio device monitor -b 9600
```

> If the board isn't auto-detected, pass the port explicitly, e.g. `--upload-port /dev/ttyUSB0`
> (or `-t upload` with `upload_port = ...` set in `platformio.ini`).

## Notes

- The example targets the **ShimmerEVM testnet** (`https://json-rpc.evm.testnet.shimmer.network`).
  To use a different chain, change the endpoint (and the bundled CA certificate, if the new
  endpoint uses a different root) in `src/main.cpp`.
- The library is pulled in via a `symlink://` dependency to the parent `r2web3` repo and to
  `r2common`, as configured in `platformio.ini`.
