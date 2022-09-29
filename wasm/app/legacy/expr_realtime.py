from re import M
import numpy as np
import pylab as pl

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
sample = load_file(file_path).get("ysum")

sampling_rate = 30
fft_size = 300
t = np.arange(0, 10.0, 1.0/sampling_rate)
ss = sample[:300]

# use sampling rate as avg window calculate average base
realtime_filter_avg_window = sampling_rate * 3
avgWindowAmplifier = 1.0 / realtime_filter_avg_window
# lower log1 as real time delay, less than 200ms walking delay
realtime_filter_delay_window= sampling_rate / 10
delayWindowAmplifier = 1.0 / realtime_filter_delay_window
# take sample amplifier 
realtime_filter_pass_window = sampling_rate / 3
passWindowAmplifier=  1.0 / realtime_filter_pass_window

motionFormer= sample[0]
motionAvgDelay = 0
motionAvgAvg = 0
motionAvgPass = 0

filteredData = []
digitalData = []
direction = 0
decending = 0
filteredData = np.zeros(300)
pidData = np.zeros(300)
direction = np.zeros(300)

#current action stage presist length
actionStepLength = 0
#stage change vulve values
vulve = 0.25
#angle direction 1 for up -1 for down 
previousDirection = 0
stepDirection = 0
descendingMin = 0
ascendingMax = 0
debugPlot = True

for i, x in enumerate(ss):
    # filteredData[i], z = signal.lfilter(b, 1, [x], zi=z)
    if i == 0:
        filteredData[i] = x
    else:
        filteredData[i] = x * 0.2 + filteredData[i-1] * 0.8
    p = filteredData[i]
    d = filteredData[i] - filteredData[i - 1]

    # direction judgement
    motionDirection = np.sign(d)
    if previousDirection!= motionDirection: 
        directionChange = True
    previousDirection = motionDirection

    # if tracking lost use current direction and next
    if stepDirection == 0:
        stepDirection = motionDirection

    # maintain max and min value in a step 
    if stepDirection == 1: 
        ascendingMax = max(ascendingMax, p)
        pidData[i] = ascendingMax
    if stepDirection == -1:
        descendingMin = min(descendingMin, p)
        pidData[i] = descendingMin

    # when direction change exame vulve value, to check if direction changed 
    if directionChange and motionDirection != stepDirection:
        if motionDirection == 1 and p - descendingMin > vulve:
            actionStepLength = 0
            stepDirection = motionDirection
            ascendingMax = p
            descendingMin = p 
            # real direction changed print
            
        if motionDirection == -1 and ascendingMax - p > vulve:
            actionStepLength = 0
            stepDirection = motionDirection
            ascendingMax = p
            descendingMin = p 
            # real direction changed print
    
    direction[i] = stepDirection
    actionStepLength = actionStepLength + 1

print(filteredData)
pl.figure(figsize=(8,4))
pl.subplot(211)
pl.plot(t[:300], ss)
pl.xlabel(u"Time(S)")
pl.title(u"WaveForm And Freq")
pl.subplot(212)
pl.plot(t[:300], direction)
pl.xlabel(u"Time(S)")
pl.subplots_adjust(hspace=0.4)
pl.show()