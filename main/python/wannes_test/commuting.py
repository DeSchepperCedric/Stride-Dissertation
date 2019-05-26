import csv
import matplotlib.pyplot as plt
import os
import shutil

from pystride.Event import Event, EventType
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


def plotNewCases(outputPrefix, files, seeds):
    """
        Plot new cases per day for a list of vaccination levels.
    """
    for file in files:
        newCases = [[]]
        for seed in seeds:
            days = []
            newCasesPerDay = []
            prevCumulativeCases = 0  # Keep track of how many cases have been recorded until current time-step
            with open(os.path.join(outputPrefix + "/" + str(file) + "/" + str(seed), "cases.csv")) as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    days.append(int(row["timestep"]))
                    cumulativeCases = int(row["cases"])
                    newCasesPerDay.append(cumulativeCases - prevCumulativeCases)
                    prevCumulativeCases = cumulativeCases
            # print(cumulativeCases)
            newCases.append(newCasesPerDay)

        del newCases[0]
        res = pd.DataFrame({'col' + str(i): newCases[i] for i in range(len(newCases))})
        # print(res)
        median = res.mean(axis=1)
        plt.plot(days, median)

    plt.xlabel("Simulation day")
    plt.ylabel("New cases per day")
    legend = []
    for i in files:
        legend.append(str(i) + "% commuting")
    plt.legend(legend)
    plt.savefig("commuting.png")
    plt.show()


def runSimulation(outputPrefix, file, seed):
    # Set up simulator
    control = PyController(data_dir="data")
    # Load configuration from file
    control.loadRunConfig(os.path.join("config", "run_generate_default.xml"))
    # Set some parameters
    print("commuting" + str(file) + ".proto")
    control.runConfig.setParameter("population_file", "commuting" + str(file) + ".proto")
    control.runConfig.setParameter("population_type", "import")
    control.runConfig.setParameter("output_prefix", outputPrefix + "/" + str(file) + "/" + str(seed))
    control.runConfig.setParameter("seeding_rate", 0.00000334)  # Seed 2 infected persons in population of 600 000
    control.runConfig.setParameter('rng_seed', seed)
    control.registerCallback(trackCases, EventType.Stepped)
    # Run simulation
    control.control()


def main():
    run_tests = True
    outputPrefix = "commuting"
    if run_tests:
        shutil.rmtree('commuting', ignore_errors=True)
    files = range(10, 110, 10)
    seeds = range(1, 10)
    # Run simulations
    if run_tests:
        for v in files:
            for seed in seeds:
                runSimulation(outputPrefix, v, seed)
    # Post-processing
    plotNewCases(outputPrefix, files, seeds)


if __name__ == "__main__":
    main()
