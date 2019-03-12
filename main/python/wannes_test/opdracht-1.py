import matplotlib.pyplot as plt
import csv

x = []
y = []

with open('StochasticAnalysis_stan_infected.csv', 'r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    columns = next(plots)

    for _ in columns:
        x.append([])


    i = 0
    for row in plots:
        for c in range(len(columns)):
            x[c].append(row[c])
        y.append(i)
        i += 1

    for c in range(len(x)):
        x[c].pop(0)

    y.pop(-1)

    print(y)


average = []
for index in range(len(x[0])):
    a = 0
    for col in x:
        a += int(col[index])
    average.append(a/len(x))



plt.plot(y, average, label='Loaded from file!')
plt.xlabel('x')
plt.ylabel('y')
plt.title('Interesting Graph\nCheck it out')
plt.legend()
plt.show()
