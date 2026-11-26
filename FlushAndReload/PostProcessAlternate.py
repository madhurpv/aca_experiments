
import numpy as np
import matplotlib.pyplot as plt

# Load raw timing measurements
timings = np.loadtxt("alternatetimings.txt")
timings = np.asarray(timings).flatten()

for i in range(len(timings)):
    if timings[i]>5000:
        timings[i] = 5000

# Initial threshold from your previous experiments
THRESHOLD = 910
THRESHOLD = 300

# Change this according to your reader's sampling interval.
# With Sleep(1), 100 samples are approximately 100 ms.
WINDOW = 100

# Basic statistics
print("Total samples:", len(timings))
print("\nTiming statistics:")
print("Minimum:", np.min(timings))
print("Maximum:", np.max(timings))
print("Median:", np.median(timings))
print("P10:", np.percentile(timings, 10))
print("P25:", np.percentile(timings, 25))
print("P75:", np.percentile(timings, 75))
print("P90:", np.percentile(timings, 90))
print("P99:", np.percentile(timings, 99))

# Identify relatively fast reloads
fast = timings < THRESHOLD

print("\nFast reload analysis:")
print("Threshold:", THRESHOLD)
print("Fast reloads:", np.sum(fast))
print("Slow reloads:", np.sum(~fast))
print("Fast reload percentage:", 100 * np.mean(fast))

# ------------------------------------------------
# 1. Timing vs sample number
# ------------------------------------------------

plt.figure(figsize=(14, 5))
plt.plot(timings, linewidth=0.7)
plt.axhline(THRESHOLD, linestyle="--",
            label=f"Threshold = {THRESHOLD}")
plt.xlabel("Sample number")
plt.ylabel("TSC ticks")
plt.title("Reload latency over time")
plt.legend()
plt.grid(alpha=0.3)
plt.tight_layout()
plt.show()

# ------------------------------------------------
# 2. Histogram
# ------------------------------------------------

# Clip only for visualization, not for statistics
limit = np.percentile(timings, 99)

plt.figure(figsize=(10, 5))
plt.hist(timings, bins=70, range=(0, limit),
         alpha=0.75, edgecolor="black")
plt.axvline(THRESHOLD, linestyle="--",
            label=f"Threshold = {THRESHOLD}")
plt.xlabel("TSC ticks")
plt.ylabel("Frequency")
plt.title("Distribution of reload latencies")
plt.legend()
plt.tight_layout()
plt.show()

# ------------------------------------------------
# 3. Rolling percentage of fast reloads
# ------------------------------------------------

fast_float = fast.astype(float)

if len(fast_float) >= WINDOW:
    rolling_fast = np.convolve(
        fast_float,
        np.ones(WINDOW) / WINDOW,
        mode="valid"
    )

    plt.figure(figsize=(14, 5))
    plt.plot(rolling_fast * 100)
    plt.xlabel("Sample number")
    plt.ylabel("Fast reloads (%)")
    plt.title(f"Rolling fast reload percentage ({WINDOW} samples)")
    plt.grid(alpha=0.3)
    plt.tight_layout()
    plt.show()

# ------------------------------------------------
# 4. Locations of fast reloads
# ------------------------------------------------

fast_indices = np.flatnonzero(fast)
gaps = np.diff(fast_indices)

print("\nFast reload locations:")
print(fast_indices)

if len(gaps) > 0:
    print("\nGaps between consecutive fast reloads:")
    print("Median gap:", np.median(gaps))
    print("Mean gap:", np.mean(gaps))
    print("First 30 gaps:", gaps[:30])

    plt.figure(figsize=(12, 4))
    plt.hist(gaps, bins=50, edgecolor="black")
    plt.xlabel("Samples between fast reloads")
    plt.ylabel("Frequency")
    plt.title("Intervals between fast reloads")
    plt.tight_layout()
    plt.show()
