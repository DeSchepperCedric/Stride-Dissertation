from sklearn.preprocessing import MinMaxScaler, StandardScaler
from pandas import Series

import pandas as pd
import numpy as np

import json
from math import sqrt

l = json.loads(open("households_flanders.json").read())
ages = pd.read_csv("ages_provinces.csv")

#ages['mul'] = np.multiply(ages['CD_AGE'], ages['MS_POPULATION'])
ant  = (10000, "Antwerpen")
vl_b = (20001, "Vlaams-Brabant")
east = (40000, "Oost-Vlaanderen")
west = (30000, "West-Vlaanderen")
limb = (70000, "Limburg")
wl_b = (20002, "Waals-Brabant")
namu = (90000, "Namen")
luik = (60000, "Luik")
hene = (50000, "Henegouwen")
luxe = (80000, "Luxemburg")

for prov in [ant, vl_b, east, west, limb, wl_b, namu, luik, hene, luxe]:
	ages_prov = ages[ages["CD_PROV_REFNIS"] == prov[0]]

	a = []
	for fam in l["households_list"]:
		for p in fam:
			a.append(p)
	test = [1,2,3,4,5]
	test = []
	for index, row in ages_prov.iterrows():
		for i in range(row["MS_POPULATION"]):
			test.append(row["CD_AGE"])

	series = Series(a)
	values = series.values
	series_prov = Series(test)
	values_prov = series_prov.values

	values = values.reshape((len(values), 1))
	values_prov = values_prov.reshape((len(values_prov), 1))

	#scaler = MinMaxScaler(feature_range=(0, 1))
	scaler = StandardScaler()
	scaler_prov = StandardScaler()

	scaler = scaler.fit(values)
	scaler_prov = scaler_prov.fit(values_prov)
	print('Mean: %f, StandardDeviation: %f' % (scaler.mean_, sqrt(scaler.var_)))
	print('Mean: %f, StandardDeviation: %f' % (scaler_prov.mean_, sqrt(scaler_prov.var_)))
	normalized = scaler.transform(values)
	normalized_prov = scaler_prov.transform(values_prov)

	inversed = scaler.inverse_transform(normalized)
	inversed_prov = scaler_prov.inverse_transform(normalized_prov)
	#for i in range(5):
	#	print(normalized_prov[i])

	hh_prov = []
	index = 0
	for fam in l["households_list"]:
		fam_prov = []
		for p in fam:
			age = int(normalized[index] * sqrt(scaler_prov.var_) + scaler_prov.mean_)
			fam_prov.append(age)
			index += 1
		hh_prov.append(fam_prov)
	with open(prov[1]+".json", "w") as f:
		f.write(json.dumps({"households_list":hh_prov}, indent=4))

