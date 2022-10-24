import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

file_path = "/Users/ember/2022/os_dev_docker/wasm/app/legacy/testx.csv"
file_path2 = "/Users/ember/2022/os_dev_docker/wasm/app/legacy/testy.csv"
file_path3 = "/Users/ember/2022/os_dev_docker/wasm/app/legacy/testz.csv"
readData = pd.read_csv(file_path,header=None)
readData2 = pd.read_csv(file_path2,header=None)
readData3 = pd.read_csv(file_path3,header=None)

data = readData.iloc[:,0].tolist()
data2 = readData2.iloc[:,0].tolist()
data3 = readData3.iloc[:,0].tolist()
xData = list(range(1,len(data)+1))
xData2 = list(range(1,len(data2)+1))
xData3 = list(range(1,len(data3)+1))

plt.subplot(311)
plt.plot(xData, data, label = "x")
plt.legend()
plt.subplot(312)
plt.plot(xData2, data2, label = "y")
plt.legend()
plt.subplot(313)
plt.plot(xData3, data3, label = "z")
plt.legend()
plt.show() 