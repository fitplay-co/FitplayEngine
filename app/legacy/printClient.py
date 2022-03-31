import websocket
import matplotlib.pyplot as pl
import asyncio
import numpy as np
import threading 
import random
import json

rawData_z = [0] * 300
watchData_z = [0] * 300
rawData_x = [0] * 300
watchData_x = [0] * 300
rawData_y = [0] * 300
watchData_y = [0] * 300
t = np.arange(0, 300)
shared_resource_lock = threading.Lock()

async def refreshBytes():
    while True:
        await asyncio.sleep(0.3)
        pl.clf()
        shared_resource_lock.acquire()        
        pl.subplot(231)
        pl.plot(t, rawData_z)
        pl.title("pixel_z")
        pl.subplot(234)
        pl.plot(t, watchData_z)
        pl.title("real_z") 
        pl.subplot(232)
        pl.plot(t, rawData_x)
        pl.title("pixel_x")
        pl.subplot(235)
        pl.plot(t, watchData_x)
        pl.title("real_x")
        pl.subplot(233)
        pl.plot(t, rawData_y)
        pl.title("pixel_y")
        pl.subplot(236)
        pl.plot(t, watchData_y)
        pl.title("real_y") 
        shared_resource_lock.release()
        pl.pause(0.01)
        pl.ioff()


def startInternal(): 
    def on_message(ws, message):
        message = json.loads(message)
        try:
            if message["monitor"] != "":
                shared_resource_lock.acquire()        
                del watchData_z[1]
                del rawData_z[1]
                del watchData_x[1]
                del rawData_x[1]
                del watchData_y[1]
                del rawData_y[1]  
                rawData_z.append(float(message["monitor"]["rawData_z"]))
                watchData_z.append(float(message["monitor"]["watchData_z"])) 
                rawData_x.append(float(message["monitor"]["rawData_x"]))
                watchData_x.append(float(message["monitor"]["watchData_x"]))
                rawData_y.append(float(message["monitor"]["rawData_y"]))
                watchData_y.append(float(message["monitor"]["watchData_y"]))  
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