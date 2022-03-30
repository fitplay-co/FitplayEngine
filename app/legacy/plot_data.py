import json
import numpy as np
import matplotlib.pyplot as plt
import os.path

file_path = "./app/test/jump1"

def load_file(name):
    if not os.path.isfile(name + ".json"):
        return {}
    with open(name + ".json", mode = 'r') as data_file:
        return json.load(data_file)

res = os.getcwd()
data = load_file(file_path).get("ysum")
t = np.arange(0, len(data))
plt.subplot(211)
plt.plot(t, data, color='b', linestyle='--', marker='o', label='discrete')
plt.subplot(212)
plt.plot(t, data)
plt.show()