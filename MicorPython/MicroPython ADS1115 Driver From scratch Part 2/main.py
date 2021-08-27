
import utime
from machine import SPI
from machine import Pin
from NRFlib import NRF
from ADSlib import DoubleJoystick    
    

nrf = NRF()
nrf.config()

nrf.modeTX()


dev = DoubleJoystick()
dev.address = 72
dev.printDevices()
dev.config()


counter = 0;
while True:
        
    counter += 1
    if counter > 10:
        counter = 0
        print(dev.joystickInputs)
        msg = "Joystick " + ", ".join( [ str(i) for i in dev.joystickInputs])
        nrf.sendMessage(msg)
        
    dev.readJoystickInputs()
    utime.sleep(0.01)



 
                           
