from sklearn.preprocessing import MinMaxScaler, StandardScaler
from pandas import Series

import pandas as pd
import numpy as np

import json
from math import sqrt


ages = pd.read_csv("TF_SOC_POP_STRUCT_2018.txt", delimiter="|")

ages.loc[ages["CD_PROV_REFNIS"] == " ", "CD_PROV_REFNIS"] = 20001
ages = ages.astype({"CD_PROV_REFNIS": int})

#VLAANDEREN
centrumsteden = [11002, 41002, 31005, 71016, 44021, 71022, 34022, 24062, 12025, 35013, 36015, 46021, 13040]

#WALLONIE
#Brussel, Aarlen, Charleroi, Luik, Namen, Bergen, La Louviere, Doornik, Verviers, Waver
centrumsteden += [21004, 81001, 52011, 62063, 92094, 53053, 55022, 57081, 63079, 25112]

ages = ages[ages["CD_MUNTY_REFNIS"].isin(centrumsteden)].groupby(["CD_PROV_REFNIS","CD_AGE"]).agg({'MS_POPULATION':"sum"})#.reset_index()
#print ages

ages.to_csv("ages_centrumsteden.csv")
