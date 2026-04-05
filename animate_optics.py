#!/usr/bin/env python3
"""Animate FDTD or BPM propagation data from OpticsCore CSV output."""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

def animate_fdtd(csv_file="fdtd_output.csv"):
    df = pd.read_csv(csv_file)
    times = df["t"].unique()
    x = df[df["t"] == times[0]]["x"].values

    fig, ax = plt.subplots()
    line, = ax.plot(x, np.zeros_like(x))
    ax.set_xlabel("x (m)")
    ax.set_ylabel("Ez")
    ax.set_title("1D FDTD Propagation")
    ax.set_ylim(-1.5, 1.5)

    def update(frame):
        t = times[frame]
        data = df[df["t"] == t]
        line.set_ydata(data["Ez"].values)
        ax.set_title(f"1D FDTD  t = {t:.3e} s")
        return line,

    ani = FuncAnimation(fig, update, frames=len(times), interval=30, blit=True)
    plt.show()

if __name__ == "__main__":
    animate_fdtd()
