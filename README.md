# LSEG Exchange Order Matching System

A high-performance order matching engine for a flower exchange market, implementing price-time priority matching algorithm.

## Overview

This C++ application simulates an electronic exchange trading system that processes orders for five flower instruments: Rose, Lavender, Lotus, Tulip, and Orchid. The system validates incoming orders, matches buy and sell orders based on price-time priority, and generates execution reports.

## Features

- **Price-Time Priority Matching**: Buy orders matched with sell orders based on best price and time of arrival
- **Multiple Instruments**: Supports 5 different flower types with independent order books
- **Order Validation**: Comprehensive validation for instrument, side, quantity, and price
- **Partial Fill Support**: Handles partial order executions with appropriate status tracking
- **Execution Reporting**: Generates detailed CSV reports of all order executions and rejections
- **Timestamp Tracking**: Millisecond-precision timestamps for all transactions

## Supported Instruments

- Rose
- Lavender
- Lotus
- Tulip
- Orchid

## Order Validation Rules

### Valid Order Requirements

| Field | Rule |
|-------|------|
| **Instrument** | Must be one of: Rose, Lavender, Lotus, Tulip, Orchid |
| **Side** | 1 (Buy) or 2 (Sell) |
| **Quantity** | ≥ 10 and < 1000, must be divisible by 10 |
| **Price** | Must be > 0 |

### Rejection Reasons

- Invalid instrument
- Invalid side
- Invalid size
- Invalid price
- Invalid quantity (non-numeric)

## Input Format

The system reads orders from a CSV file with the following structure:

```csv
Client Order ID,Instrument,Side,Quantity,Price
aa001,Rose,1,100,55.55
aa002,Lavender,2,200,45
```

### CSV Fields

- **Client Order ID**: Unique identifier for the order (string)
- **Instrument**: Flower type (Rose, Lavender, Lotus, Tulip, Orchid)
- **Side**: 1 = Buy, 2 = Sell
- **Quantity**: Order size (must be 10-990, divisible by 10)
- **Price**: Price per unit (numeric)

## Output Format

The system generates an execution report CSV with the following columns:

```csv
Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction time
ord1,aa001,Rose,1,New,100,55.55,Accepted,2025.11.22-14.30.45-123
```

### Execution Status Types

- **New**: Order accepted and added to order book (no match found)
- **Fill**: Order completely filled
- **PFill**: Order partially filled
- **Rejected**: Order rejected due to validation failure

## How It Works

### Order Matching Algorithm

1. **Incoming Order**: System receives a new order from CSV
2. **Validation**: Order is validated against business rules
3. **Matching**: 
   - Buy orders try to match with sell orders (buy price ≥ sell price)
   - Sell orders try to match with buy orders (sell price ≤ buy price)
   - Matches execute at the resting order's price (maker price)
4. **Execution**: Matched quantities are executed and reported
5. **Remaining Quantity**: Any unfilled quantity is added to the order book
6. **Sorting**: Order book is sorted (buy: highest to lowest, sell: lowest to highest)

### Example Matching Scenario

```
Order Book (Rose):
Buy:  100 @ $55
Sell: 150 @ $50

New Buy Order: 200 @ $60
→ Matches with Sell @ $50 for 150 units (execution price: $50)
→ Remaining 50 units added to buy side @ $60
```

## Building and Running

### Prerequisites

- C++ compiler with C++11 support (gcc/g++ or MSVC)
- Input CSV file (`orders_final.csv`)

### Compilation

**Using GCC/G++:**
```bash
g++ -std=c++11 ExchangeApplication.cpp -o ExchangeApplication
```

**Using VS Code Task:**
```
Ctrl+Shift+B (select "C/C++: g++.exe build active file")
```

### Execution

```bash
./ExchangeApplication
```

The program will:
1. Read orders from `orders_final.csv`
2. Process each order sequentially
3. Generate `execution_rep_final.csv` with results

## Project Structure

```
LSEG-Coding-Competition/
├── ExchangeApplication.cpp    # Main application code
├── Orders.csv                  # Sample input file
├── orders_final.csv           # Production input file
├── execution_rep_final.csv    # Generated execution report
└── README.md                  # This file
```

## Code Architecture

### Classes

#### `Order`
Represents a single order with attributes:
- Client Order ID
- Instrument
- Side (Buy/Sell)
- Quantity
- Price
- System-generated Order ID
- Timestamp

#### `OrderBook`
Manages buy and sell orders for a single instrument:
- **Private Members**: 
  - `buyOrders` vector (sorted high to low)
  - `sellOrders` vector (sorted low to high)
- **Key Methods**:
  - `addOrder()`: Validates and processes incoming orders
  - `processOrder()`: Matches orders and updates order book
  - `executeTrade()`: Executes matched trades
  - `validOrder()`: Validates order parameters
  - `sortOrders()`: Maintains price-time priority

### Global Order Books

Five independent `OrderBook` instances:
- `roseOrderBook`
- `lavenderOrderBook`
- `lotusOrderBook`
- `tulipOrderBook`
- `orchidOrderBook`

## Performance Characteristics

- **Time Complexity**: O(n log n) per order (due to sorting)
- **Space Complexity**: O(n) where n is number of active orders
- **Processing**: Sequential order processing (single-threaded)

## Potential Improvements

- Use priority queues for O(log n) insertions instead of vector sorting
- Implement order cancellation functionality
- Add order modification support
- Implement multi-threading for concurrent order processing
- Add real-time order book visualization
- Implement market order support (currently only limit orders)
- Add stop-loss and stop-limit order types

## Sample Output

```csv
ord1,aa001,Rose,1,Fill,100,55,Accepted,2025.11.22-14.30.45-123
ord2,aa002,Rose,2,New,200,60,Accepted,2025.11.22-14.30.45-234
ord3,aa003,Lavender,1,PFill,50,45,Accepted,2025.11.22-14.30.45-345
ord4,aa004,Rose,1,Rejected,5,50,Invalid size,2025.11.22-14.30.45-456
```

## License

This project was developed for the LSEG Coding Competition.

## Author

Senula

## Date

November 2025
