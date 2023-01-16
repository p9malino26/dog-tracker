from flask import Flask, render_template, jsonify
import os
import json
import datetime
import sys

test = sys.argv.__len__() > 1 

if not test: from pyLoraRFM9x import LoRa, ModemConfig
else: import threading
from time import sleep



payload_ = None
app = Flask(__name__, )
lastReceptionTime = -1

def set_status(status):
    global status_
    status_ = status


def parsePayload(payload):    
    if not payload:
        return json.dumps({'status':'nsi'})

    message = payload['message']
    tokens = message.split(' ')
    status = tokens[0]
    out = {'status':status}

    if len(tokens) > 1:
        out['latitude'] = float(tokens[1])
        out['longitude'] = float(tokens[2])
        out['posUpdateTime'] = int(tokens[3])

    out['lastReceptionTime'] = int((datetime.datetime.now() - lastReceptionTime).total_seconds())
    out['rssi'] = payload['rssi']
    
    return json.dumps(out)

@app.route('/')
def start_page_():
    return render_template("index.html")

@app.route("/data", methods=["POST"])
def on_refresh_():
    return parsePayload(payload_)

@app.route("/shutdown", methods=["POST"])
def shutdown_():
    if test:
        return os.system("touch ~/success.txt")
    else:
        return os.system("sudo sh /home/ryan/dogtracker/teardown.sh")

def processPayload(payload):
    global payload_, lastReceptionTime
    payload_ = payload
    lastReceptionTime = datetime.datetime.now()

def on_recv(payload):
    print("From:", payload.header_from)
    print("Received:", payload.message)
    print("RSSI: {}; SNR: {}".format(payload.rssi, payload.snr))
    print("Time:", datetime.datetime.now())
    payload = {'message':payload.message.decode('utf-8').lower(), 'rssi': payload.rssi, 'snr': payload.snr} 
    processPayload(payload)

def main():
    lora = LoRa(0, 5, 0x21, freq=868, reset_pin=26, receive_all=True, acks=False, modem_config=ModemConfig.Bw125Cr45Sf128)
    lora.on_recv = on_recv
    lora.set_mode_rx()

    app.run(ssl_context='adhoc',host="0.0.0.0") 

    print("Closing...")
    lora.close()
    
def sendTestMessages():
    testMessages = (
            "loc 51.25513 -0.58730 0",
            "loc 51.25486 -0.58809 0",
            )

    while True:
        for msg in testMessages:
            if msg == 'nsi':
                processPayload(None)
            else:
                p = {'message': msg, 'rssi': -67}
                processPayload(p)
            sleep(3)

if __name__ == '__main__':
    if test:
        threading.Thread(target=sendTestMessages).start()
        app.run(host="0.0.0.0")
    else:
        main()
