import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

file_path = "./app/legacy/expr_data/step_rate_v1.csv"
file_path2 = "./app/legacy/expr_data/step_rate_v2.csv"
readData = pd.read_csv(file_path,header=None)
readData2 = pd.read_csv(file_path2,header=None)

data = readData.iloc[:,0].tolist()
data2 = readData2.iloc[:,0].tolist()
xData = list(range(1,len(data)+1))
xData2 = list(range(1,len(data2)+1))

plt.subplot(211)
plt.plot(xData, data)
plt.subplot(212)
plt.plot(xData2, data2)
plt.show() 