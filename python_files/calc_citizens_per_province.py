import pandas

df = pandas.read_csv("../main/resources/data/flanders_cities.csv")
print df.groupby(["province"]).agg({"population":"sum"})

print df.groupby(["province"]).agg({"population":"sum"}).agg("sum")
