import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

file_path = "./app/legacy/data.csv"
readData = pd.read_csv(file_path,header=None)
print(readData)
data = readData.iloc[:,0].tolist()
xData = list(range(1,len(data)+1))
 
plt.plot(xData, data)
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