import pandas as pd
import numpy as np
from sklearn.linear_model import LinearRegression
from yahoofinancials import YahooFinancials
import matplotlib.pyplot as plt
from datetime import datetime

def get_stock_data(stock, start_date, end_date):
    yahoo_financials = YahooFinancials(stock)
    stats = yahoo_financials.get_historical_price_data(start_date, end_date, "daily")
    prices = stats[stock]["prices"]
    df = pd.DataFrame(prices)
    df = df[['open', 'low', 'high', 'close']]
    return df

def predictor(data, months):
    train_size = len(data) - months
    df_train = data.iloc[:train_size, :]
    df_validation = data.iloc[train_size:, :]

    X_train = df_train.iloc[:, :-1]
    y_train = df_train.iloc[:, -1]

    model = LinearRegression()
    model.fit(X_train, y_train)

    X_validation = df_validation.iloc[:, :-1]
    y_validation = df_validation.iloc[:, -1]

    y_pred = model.predict(X_validation)

    print("\n\nPrediction")
    print(y_pred)

    print("\nOriginal")
    print(y_validation.values)

    print("\nPredicted")
    print(y_pred)

    return y_pred, y_validation.values

if __name__ == "__main__":
    stock = input("Which stock? ").upper()
    start_date = "2018-01-01"
    end_date = datetime.now().strftime("%Y-%m-%d")
    
    data = get_stock_data(stock, start_date, end_date)
    print(f"No. of data points: {len(data)}")

    months = int(input("How many months for validation? "))
    y_pred, y_validation = predictor(data, months)

    plt.figure(figsize=(12, 6))
    plt.plot(y_validation, label="Original")
    plt.plot(y_pred, label="Predicted")
    plt.xlabel("Days")
    plt.ylabel("Stock Price")
    plt.title(f"{stock} Stock Price Prediction")
    plt.legend()
    plt.show()
