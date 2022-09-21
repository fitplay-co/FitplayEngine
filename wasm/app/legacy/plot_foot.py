import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from one_euro_filter import OneEuroFilter

file_path = "./app/legacy/expr_data/re1_foot.csv"
readData = pd.read_csv(file_path,header=None)
# print(readData)
fpmStopCount = 0
fpmStopCount2 = 0
frameShiftFilterCount = 0

data = readData.iloc[:,0].tolist()
mean = list(range(1,len(data)+1))
res = list(range(1,len(data)+1))
mean[0] = data[0]
res[0] = 0
xData = list(range(1,len(data)+1))

pre = 0
current = 0
threshold = 0.01
# threshold = 0.02

for i in range(1,len(data)) :
    # if current == -1:
    #     downCount = downCount + 1
    #     if downCount > 10:
    #         current = 0
    # else:
    #     downCount = 0
    mean[i] = mean[i-1] * 0.85 + data[i] * 0.15
    if data[i] - mean[i] > threshold:
        if current != -1:
            if frameShiftFilterCount > 3 and current == 1:
                current = -1
            else :
                frameShiftFilterCount = frameShiftFilterCount + 1
        else :
            frameShiftFilterCount = 0

    if data[i] - mean[i] < -threshold:
        if current != 1:
            if frameShiftFilterCount > 3:
                current = 1
            else :
                frameShiftFilterCount = frameShiftFilterCount + 1
        else :
            frameShiftFilterCount = 0

    if abs(data[i] - mean[i]) < threshold:
        if current != 0:
            if frameShiftFilterCount > 6:
                current = 0
            else :
                frameShiftFilterCount = frameShiftFilterCount + 1
        else :
            frameShiftFilterCount = 0

    if current != 0 :
        if abs(data[i] - mean[i]) < threshold :
            fpmStopCount = fpmStopCount + 1
            if fpmStopCount > 15 :
                current = 0
        else :
            fpmStopCount = 0


    res[i] = current
# Decreasing the minimum cutoff frequency decreases slow speed jitter
# Increasing the speed coefficient decreases speed lag
min_cutoff = 0.01
beta = 1.2
one_euro_filter = OneEuroFilter(
    xData[0], data[0],
    min_cutoff=min_cutoff,
    beta=beta
)
euro_mean = list(range(1,len(data)+1))
for i in range(1,len(data)) :
    euro_mean[i] = one_euro_filter(xData[i], data[i])
euro_mean[0] = data[0]
print(mean)
plt.subplot(211)
plt.plot(xData, data, label = "noisy")
plt.plot(xData, euro_mean, label = "euro")
plt.plot(xData, mean, label = "low_pass")
plt.legend()
plt.subplot(212)
plt.plot(xData, euro_mean, label = "euro")
plt.plot(xData, mean, label = "low_pass")
plt.legend()
plt.show() 