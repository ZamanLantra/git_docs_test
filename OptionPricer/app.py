from flask import Flask, render_template, request, jsonify
import numpy as np
from pricing import price_grid, black_scholes_greeks

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/heatmap_data', methods=['POST'])
def heatmap_data():
    data = request.json
    spot = float(data['spot'])
    rate = float(data['rate'])
    volatility = float(data['volatility'])
    option_type = data['type']
    strike_low = float(data.get('strike_low', 80))
    strike_high = float(data.get('strike_high', 120))
    strike_step = int(data.get('strike_step', 9))
    maturity_low = float(data.get('maturity_low', 0.05))
    maturity_high = float(data.get('maturity_high', 1.0))   
    maturity_step = int(data.get('maturity_step', 20))

    strikes = np.arange(strike_low, strike_high, strike_step)
    maturities = np.linspace(maturity_low, maturity_high, maturity_step)

    K, T, prices = price_grid(spot, rate, volatility, option_type, strikes, maturities)
    
    print(f'Spot Price: {spot}, Rate: {rate}, Volatility: {volatility}, Option Type: {option_type}')
    print(f'strike_low: {strike_low}, strike_high: {strike_high}, strike_step: {strike_step}')
    print(f'maturity_low: {maturity_low}, maturity_high: {maturity_high}, maturity_step: {maturity_step}')

    atm_strike = min(strikes, key=lambda x: abs(x - spot))
    shortest_maturity = maturities[0]
    greeks = black_scholes_greeks(spot, atm_strike, shortest_maturity, rate, volatility, option_type)

    return jsonify({
        'z': prices.tolist(),
        'x': strikes.tolist(),
        'y': maturities.tolist(),
        'greeks': greeks,
        'atm_strike': atm_strike,
        'shortest_maturity': shortest_maturity
    })

if __name__ == '__main__':
    app.run(debug=True)
