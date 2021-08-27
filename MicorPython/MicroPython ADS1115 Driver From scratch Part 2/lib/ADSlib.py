import utime
from machine import Pin
from machine import I2C



class DoubleJoystick:
    def __init__(self, port=1, freq=400000, scl=15, sda=14):
        self.address = 0
        self.dev = I2C( port, freq=freq, scl=Pin(scl), sda=Pin(sda))
        
        self.joystickInputs = [0,0,0,0]
        self.currentChannel = 0
        
    def printDevices(self):
        devices = self.dev.scan()
        for device in devices:
            print([device, bin(device)])
            
        
    def readReg(self, reg):
        reg = [0b00000011 & reg]
        self.dev.writeto(self.address,bytearray(reg))
        result = self.dev.readfrom(self.address, 2)
        
        return result[0]<<8 | result[1]
    
    def config(self, os=1, channel=0, gain=0b1, mode=1, rate=0b100):
        config = os<<15 | (channel+4)<<12 | gain<<9 | mode<<8 | rate<<5 | 3
        
        config = [int(config>>i & 0xff) for i in (8,0)]
        #print(config)        
        self.dev.writeto(self.address, bytearray([1] + config))
    
    def readJoystickInputs(self):
        if not self.readReg(1) & 1<<15:
            return
        
        result = self.readReg(0)
        
        self.joystickInputs[self.currentChannel] = -1 if result > 20000 else 1 if result < 8000 else 0
        
        self.currentChannel += 1
        if self.currentChannel >= 4: self.currentChannel = 0
        
        self.config(channel=self.currentChannel)
        
        pass
    
    
    
    
    
    
    
    
    
    
    
