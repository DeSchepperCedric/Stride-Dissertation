import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("StochasticAnalysis_stan_infected.csv")

transposedData = data.T

mstd = transposedData.std()
ma = transposedData.mean()

plt.figure()
plt.plot(mstd.index, mstd, 'k', label="standard deviation")
plt.plot(ma.index, ma, 'b', label="mean")
plt.fill_between(mstd.index, ma - 2 * mstd, ma + 2 * mstd, color='b', alpha=0.2)
plt.show()


import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("StochasticAnalysis_stan_infected.csv")

data.plot(legend=False)
plt.show()