from pyLoraRFM9x import LoRa, ModemConfig
from time import sleep

MODE='rx'
success=False
# This is our callback function that runs when a message is received
def on_recv(payload):
    print("From:", payload.header_from)
    print("Received:", payload.message)
    print("RSSI: {}; SNR: {}".format(payload.rssi, payload.snr))
    global success
    success = True

# Use chip select 1. GPIO pin 5 will be used for interrupts and set reset pin to 25
# The address of this device will be set to 255
lora = LoRa(0, 5, 0x21, freq=868, reset_pin=26, receive_all=True, acks=False, modem_config=ModemConfig.Bw125Cr45Sf128)
print("Init done")
lora.on_recv = on_recv
if MODE == 'rx':
    lora.set_mode_rx()
    try:
        while True:
            sleep(0.5)
    except KeyboardInterrupt:
        pass
else:
    lora.set_mode_tx()    

    # Send a message to a recipient device with address 10
    # Retry sending the message twice if we don't get an  acknowledgment from the recipient
    try:
        while True:
            print("Sending")
            message = "This is a test message"
            status = lora.send_to_wait(message, 10, retries=2)
            sleep(2)
    except KeyboardInterrupt:
        pass

print("Closing...")
lora.close()
