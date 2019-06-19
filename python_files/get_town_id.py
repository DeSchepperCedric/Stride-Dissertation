import pandas as pd

ages = pd.read_csv("TF_SOC_POP_STRUCT_2018.txt", delimiter="|")
ages.loc[ages["CD_PROV_REFNIS"] == " ", "CD_PROV_REFNIS"] = 20001
ages = ages.astype({"CD_PROV_REFNIS": int})

#print ages[ages["TX_MUNTY_DESCR_NL"] == "Brussel"]
#print ages[ages["CD_MUNTY_REFNIS"] == 25112]

print set(ages[ages["TX_MUNTY_DESCR_NL"] == "Aarlen"].CD_MUNTY_REFNIS)
