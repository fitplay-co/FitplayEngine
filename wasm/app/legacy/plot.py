import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

file_path = "/Users/ember/2022/os_dev_docker/wasm/app/legacy/fitplay1019test3.csv"
file_path2 = "/Users/ember/2022/os_dev_docker/wasm/app/legacy/fitplay1019test3.csv"
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