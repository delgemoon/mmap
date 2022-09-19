### TRADING ROBOTS SUITE

1. high performance trading suite including connector, arbitrage, market maker, hedger. data collector, simulate machine engineer, back-test utilities based in C++20, ..etc...
2. Design: Event souring approach
   1. apps shall run as monolithic app or micro app
   2. apps shall support local or remote broker.
   3. apps should ease to change database.

### Features 

### version 1

#### Connectors

- FTX
- Binance
- Kraken

#### Strategies

- SPOT Pure market maker
- SPOT Cross exchange market maker

#### Machine Learning Algorithms

1. Price prediction
2. Return P/L
3. Buy/sell signal
4. Portfolio optimization
5. Efficient Execution

###### Momentum Trading model (Price Prediction)

- Create a prediction model (price prediction) that can help to trade price difference between two assets

`Basic = Price of stock  - Price of Future`

Feature(X)
- ETH data at one-minute trading intervals for trading days over one month
1. ETH Bid Price, Ask price, bid volume, ask volume
2. ETH future bid price, ask price, bid volume, ask volume
3. ETH VWAP
4. ETH Future VWAP

VWAP = (bid_price * ask_volume + ask_price * ask_volume) / (ask_volume + bid_volume)


###### Back-testing