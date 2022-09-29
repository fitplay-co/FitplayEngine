import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

file_path = "./app/legacy/expr_data/re1_diffz.csv"
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
threshold = 0.03

for i in range(1,len(data)) :
    if data[i] > 1:
        data[i] = 0
    # if data[i] > 0.07:
    #     data[i] = 0.07
    if current == -1:
        downCount = downCount + 1
        if downCount > 10:
            current = 0
    else:
        downCount = 0
    mean[i] = mean[i-1] * 0.95 + data[i] * 0.05
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
            if frameShiftFilterCount > 3 and mean[i] < 0.05:
                current = 1
            else :
                frameShiftFilterCount = frameShiftFilterCount + 1
        else :
            frameShiftFilterCount = 0

    if abs(data[i] - mean[i]) < threshold:
        if current != 0:
            if frameShiftFilterCount > 5:
                current = 0
            else :
                frameShiftFilterCount = frameShiftFilterCount + 1
        else :
            frameShiftFilterCount = 0

    if current != 0 :
        if abs(mean[i] - 0.02) < 0.01:
            fpmStopCount2 = fpmStopCount2 + 1
            if fpmStopCount2 > 15 :
                current = 0
        else :
            fpmStopCount2 = 0
        if abs(data[i] - mean[i]) < threshold :
            fpmStopCount = fpmStopCount + 1
            if fpmStopCount > 15 :
                current = 0
        else :
            fpmStopCount = 0


    res[i] = current


print(mean)
plt.subplot(211)
plt.plot(xData, data)
plt.subplot(212)
plt.plot(xData, res)
plt.show() 

# import json
# import numpy as np
# import matplotlib.pyplot as plt
# import os.path

# file_path = "./app/test/jump1"

# def load_file(name):
#     if not os.path.isfile(name + ".json"):
#         return {}
#     with open(name + ".json", mode = 'r') as data_file:
#         return json.load(data_file)

# res = os.getcwd()
# data = load_file(file_path).get("ysum")
# t = np.arange(0, len(data))
# plt.subplot(211)
# plt.plot(t, data, color='b', linestyle='--', marker='o', label='discrete')
# plt.subplot(212)
# plt.plot(t, data)
# plt.show()