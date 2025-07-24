# GoQuant Stablecoin Depeg Tracker

A C++ backend application with REST API to monitor stablecoin price depegging risk using on-chain data, price feeds, and historical analysis.

---

## Introduction

This project tracks the stability of popular stablecoins (USDT, USDC, DAI, FRAX, BUSD) by analyzing their real-time prices, on-chain large transfers, and historic price/risk trends.  
It uses multiple data sources with risk scoring and alerts to help detect potential depegging events early.

The application includes a command-line interface for interactive use, as well as a REST API built with the Crow framework for integration and automation.

---

## Features

- Real-time price monitoring from multiple exchanges, with aggregated average price
- On-chain large transfer detection via Etherscan API
- Historical price & risk tracking with moving average, standard deviation, and trend/risk alarms
- Risk scoring based on price deviation and large transfer activity
- Alerts system to notify on risk threshold breaches
- REST API exposing:
  - `/` server health check
  - `/price` all tracked coin prices
  - `/price/<coin>` price data for a specific coin
  - `/onchain/<coin>` recent large on-chain transfers for a coin
  - `/history/<coin>` historical metrics and risk indicators for a coin

---

## Dependencies & Installation (Using vcpkg)

### 1. Install [vcpkg](https://github.com/microsoft/vcpkg)

Clone and bootstrap vcpkg:

```bash
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
```
Linux/macOS
```bash
./bootstrap-vcpkg.sh
```
Windows PowerShell
```bash
.\bootstrap-vcpkg.bat
```
(Optional) Add vcpkg to your PATH or integrate with your shell.

### 2. Install Required Libraries

Install dependencies through vcpkg:
```bash
vcpkg install nlohmann-json
vcpkg install crow
vcpkg install curl
```

For Windows x64, specify the triplet:
```bash
vcpkg install nlohmann-json:x64-windows crow:x64-windows curl:x64-windows
```

### 3. Integrate vcpkg with CMake

Run:

```bash
vcpkg integrate install
```

### 4. Build the Project Using CMake

From the project root:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
---

## Running the Application

Run the main executable from the build directory:
```bash
 .\Debug\StablecoinTracker.exe
```

- Follow the interactive prompt to select the stablecoin to track and desired output modules.
- The CLI will display real-time prices, on-chain transfer summaries, risk alerts, and historical insights.

### API Server Mode

The CLI and server code are integrated in `main.cpp`. To start the API server:

- Launch the app (same executable).
- The API server runs concurrently.
- The server listens on `http://localhost:8080`.

### Available API Routes

| Route                 | Description                                |
|-----------------------|--------------------------------------------|
| `/`                   | Health check, returns `"Server is running"` |
| `/price`              | Returns price data for all tracked coins    |
| `/price/<coin>`       | Returns price data for a specific stablecoin (e.g., `/price/USDT`) |
| `/onchain/<coin>`     | Returns recent large transfers on-chain for a coin (threshold $50k) |
| `/history/<coin>`     | Returns historical price and risk metrics for a coin |

---

## Configuration

- **Etherscan API key:** Update the constant in `ChainMonitor.cpp`:
```bash
const string ETHERSCAN_API_KEY = "YOUR_API_KEY_HERE";
```
- **CoinGecko & CoinCap API key:** Update the constant in `PriceMonitor.cpp`:
```bash
const string COINGECKO_API_KEY = "YOUR_API_KEY_HERE";
const string COINCAP_API_KEY = "YOUR_API_KEY_HERE";
```

- **Ports:** HTTP server listens on port `8080` by default (change in `server.cpp`).

**Limits:**
- History entries saved capped at 100.
- Alerts capped at 100 entries.

---

## Notes

- Make sure the application can read/write `history.json` and `alerts.json` in the working directory.
- API accepts coin symbols case-insensitively (converts to uppercase internally).

---

## Author

K4R-R  
[GitHub Repository](https://github.com/K4R-R/GoQuant-Stablecoin-Depeg)
