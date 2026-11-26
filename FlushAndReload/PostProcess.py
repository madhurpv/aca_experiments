import numpy as np
import matplotlib.pyplot as plt

active = np.loadtxt("active.txt")
idle = np.loadtxt("idle.txt")

for name, data in [
    ("Continuous writing", active),
    ("No writing", idle)
]:
    print("\n", name)
    print("Median:", np.median(data))
    print("P10:", np.percentile(data, 10))
    print("P25:", np.percentile(data, 25))
    print("P75:", np.percentile(data, 75))
    print("P90:", np.percentile(data, 90))
    print("P99:", np.percentile(data, 99))

# Common plotting range; retain all data for statistics.
limit = np.percentile(
    np.concatenate([active, idle]), 99
)

bins = np.linspace(0, limit, 80)

plt.hist(active, bins=bins, density=True,
         alpha=0.5, label="Continuous writing")

plt.hist(idle, bins=bins, density=True,
         alpha=0.5, label="No writing")

plt.xlabel("TSC ticks")
plt.ylabel("Density")
plt.title("Flush+Reload Timing Distribution")
plt.legend()
plt.show()