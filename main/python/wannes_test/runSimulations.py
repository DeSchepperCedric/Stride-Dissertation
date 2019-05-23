import os
import csv

from pystride.Event import EventType
from pystride.PyController import PyController


def trackCases(simulator, event):
    """
        Callback function to track cumulative cases
        after each time-step.
    """
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, "cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases" ]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})


def runSimulation(outputPrefix, simulation):
    # Set up simulator
    control = PyController(data_dir="data")
    # Load configuration from file
    control.loadRunConfig(os.path.join("config", "run_default.xml"))
    # Set some parameters
    control.runConfig.setParameter("output_prefix", outputPrefix + str(simulation))
    control.registerCallback(trackCases, EventType.Stepped)

    # Run simulation
    control.control()


def main():
    run_tests = False
    outputPrefix = "Simulations/sim"
    simulations = range(0, 10)
    # Run simulations
    for v in simulations:
        runSimulation(outputPrefix, v)


if __name__ == "__main__":
    main()
