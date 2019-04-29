import csv
import matplotlib.pyplot as plt
import os
import shutil

from pystride.Event import EventType
from pystride.PyController import PyController
import pandas as pd


def trackCases(simulator, event):
    """
        Callback function to track cumulative cases
        after each time-step.
    """
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, "cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})


def plotNewCases(outputPrefix, immune, seeds, extinctionLevel):
    """
        Plot new cases per day for a list of vaccination levels.
    """
    for v in immune:
        newCases = [[]]
        for seed in seeds:
            days = []
            newCasesPerDay = []
            prevCumulativeCases = 0  # Keep track of how many cases have been recorded until current time-step
            with open(os.path.join(outputPrefix + "_" + str(v) + "/" + str(seed), "cases.csv")) as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    days.append(int(row["timestep"]))
                    cumulativeCases = int(row["cases"])
                    newCasesPerDay.append(cumulativeCases - prevCumulativeCases)
                    prevCumulativeCases = cumulativeCases
            # print(cumulativeCases)
            if (cumulativeCases > extinctionLevel):
                newCases.append(newCasesPerDay)

        del newCases[0]
        res = pd.DataFrame({'col' + str(i): newCases[i] for i in range(len(newCases))})
        # print(res)
        median = res.mean(axis=1)
        plt.plot(days, median)

    plt.xlabel("Simulation day")
    plt.ylabel("New cases per day")
    legend = []
    for i in immune:
        legend.append(str(i) + "% immune")
    plt.legend(legend)
    plt.savefig("Immunitylevel.png")
    plt.show()


def runSimulation(outputPrefix, immune, seed):
    # Set up simulator
    control = PyController(data_dir="data")
    # Load configuration from file
    control.loadRunConfig(os.path.join("config", "outbreak_2019_estimates.xml"))
    # Set some parameters
    control.runConfig.setParameter("output_prefix", outputPrefix + "_" + str(immune) + "/" + str(seed))
    control.runConfig.setParameter('immunity_rate', immune / 100)
    control.runConfig.setParameter('rng_seed', seed)
    control.registerCallback(trackCases, EventType.Stepped)
    control.runConfig.setParameter("vaccine_rate", 0)
    # Run simulation
    control.control()


def main():
    run_tests = False
    outputPrefix = "immunityTests/Demo"
    if run_tests:
        shutil.rmtree('immunityTests', ignore_errors=True)
    immunelevels = range(69, 75)
    seeds = range(1, 20)
    # Run simulations
    if run_tests:
        for v in immunelevels:
            for seed in seeds:
                runSimulation(outputPrefix, v, seed)
    # Post-processing
    plotNewCases(outputPrefix, immunelevels, seeds, 300)


if __name__ == "__main__":
    main()
