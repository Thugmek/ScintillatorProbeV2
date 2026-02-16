import matplotlib.pyplot as plt
from line_intersect import Line
import math
import fit_algorithm

from area import get_area

samples = []
times = []

with open(f"../peak-shapes/20260211_0012.csv") as f:
    for line in f.readlines():
        try:
            samples.append(-float(line.split(",")[1].strip()))
            times.append(float(line.split(",")[0].strip()))
        except Exception as e:
            pass


area = get_area(samples)
print(f"Area: {area}")

"""
fig, subplots = plt.subplots(4,4)
subplot_row = [
    subplots[0][0],
    subplots[0][1],
    subplots[0][2],
    subplots[0][3],

    subplots[1][0],
    subplots[1][1],
    subplots[1][2],
    subplots[1][3],

    subplots[2][0],
    subplots[2][1],
    subplots[2][2],
    subplots[2][3],

    subplots[3][0],
    subplots[3][1],
    subplots[3][2],
    subplots[3][3],
]
"""

fig, subplots = plt.subplots(2,2)
subplot_row = [
    subplots[0][0],
    subplots[0][1],
    subplots[1][0],
    subplots[1][1],
]

step = int(100/6)

first_h = None
for offset in range(step):
    samples_decimated = []
    times_decimated = []
    print(f"offset: {offset}")
    for i in range(int(len(samples)/step)-1):
        samples_decimated.append(samples[i*step+offset])
        times_decimated.append(i*step+offset)

    for i in range(len(samples_decimated)-1):
        if samples_decimated[i] > 0.02 and samples_decimated[i] > samples_decimated[i+1]:
            rising_half = samples_decimated[:i]
            falling_half = samples_decimated[i+1:]
            l1 = Line(i-2,rising_half[-2],rising_half[-1]-rising_half[-2])
            l2 = Line(i + 1, falling_half[0], falling_half[1] - falling_half[0])
            center_sample = i
            break

    pkh = l1.intersection(l2)[1]
    if first_h == None:
        first_h = pkh
    error = ((pkh/first_h)-1)*100

    sample_times = []
    for i in range(len(samples)):
        sample_times.append((i - offset) / step)

    if offset%4 == 0:
        indx = int(offset/4)
        subplot_row[indx].plot(sample_times,samples, label="real", linewidth=3)
        subplot_row[indx].plot(samples_decimated, marker="+", linewidth=0, label="sampled", markersize=15)

        for model in [fit_algorithm.Landau(), fit_algorithm.TwoExp(), fit_algorithm.Levy(), fit_algorithm.ParabolicExponential()]:
            f = fit_algorithm.fit_function(samples_decimated,model)
            ftd = []
            for t in sample_times:
                ftd.append(f(t))
            subplot_row[indx].plot(sample_times, ftd, label=f"fitted ({model.name})")

subplots[0][1].legend()
plt.show()
