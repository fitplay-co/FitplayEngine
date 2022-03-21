import websocket
import matplotlib.pyplot as pl
import asyncio
import numpy as np
import threading 
import random
import json

rawData = [0] * 300
watchData = [0] * 300
t = np.arange(0, 300)
shared_resource_lock = threading.Lock()

async def refreshBytes():
    while True:
        await asyncio.sleep(0.3)
        pl.clf()
        shared_resource_lock.acquire()        
        pl.subplot(211)
        pl.plot(t, rawData)
        pl.subplot(212)
        pl.plot(t, watchData)
        shared_resource_lock.release()
        pl.pause(0.01)
        pl.ioff()


def startInternal(): 
    def on_message(ws, message):
        message = json.loads(message)
        try:
            if message["monitor"] != "":
                shared_resource_lock.acquire()        
                del watchData[1]
                del rawData[1]
                rawData.append(float(message["monitor"]["rawData"]))
                watchData.append(float(message["monitor"]["watchData"]))
                shared_resource_lock.release()        
        except KeyboardInterrupt:
            print("Closed")


    def on_error(ws, error):
        print(ws)
        print(error)


    def on_close(ws):
        print(ws)
        print("### closed ###")


    def on_open(ws):
        ws.send("{\"type\":\"application_client\"}")


    ws = websocket.WebSocketApp("ws://127.0.0.1:8181",
                                on_message=on_message,
                                on_error=on_error,
                                on_close=on_close,
                                on_open=on_open)

    wst = threading.Thread(target=ws.run_forever)
    wst.daemon = True
    wst.start()

    loop = asyncio.get_event_loop()
    task_1 = loop.create_task(refreshBytes())
    loop.run_until_complete(task_1)
    
startInternal()
