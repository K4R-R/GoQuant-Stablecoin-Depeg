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