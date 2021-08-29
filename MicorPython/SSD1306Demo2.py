import utime
from machine import Pin
from machine import I2C
import math
import random

class SSD:
    def __init__(self, port=0, freq=400000, scl=17, sda=16):
        self.address = 0
        self.dev = I2C(port, freq=freq,scl=Pin(scl),sda=Pin(sda))
        self.pixelsToClear = []
        
    def printDevices(self):
        devices = self.dev.scan()
        for device in devices:
            print(device)
            
    def sendCommand(self, cmd, data):
        if type(data) != list: data = [data]
        
        command = [0b11000000 & cmd] + data
        
        self.dev.writeto(self.address, bytearray(command))
        
    def powerUp(self):
        
        self.sendCommand(0b10000000, 0xa4)
        self.sendCommand(0b00000000, [0xaf,0x8d,0x14])
    
    def clear(self, on=255):
        for page in range(0xB0, 0xb8):
            self.sendCommand(0b10000000, page)
            self.sendCommand(0b00000000, [0x00,0x10])
           
            mem = [on for i in range(0,130)]
            self.sendCommand(0b01000000,mem)
            
    def plotPixel(self, x=10, y=10, on=1):
        
        xl = int(x & 0x0f)
        xh = int((x>>4 & 0x0f) | 0b00010000)
        
        pixel = on<<(y%7)
        
        page = int(( y - (y%7))/7) | 0xb0
        
        self.sendCommand(0b10000000, page)
        self.sendCommand(0b00000000, [xl,xh])
        
        self.sendCommand(0b01000000,pixel)
                
    def writeObject(self,x,y,letter,on=1):
        #self.sendCommand(0b10000000, 0xb1)
        #self.sendCommand(0b00000000, [0x00,0x10])
        self.plotPixel(x,y,on=0)
        if on:
            self.sendCommand(0b01000000,letter)
        else:
            self.sendCommand(0b01000000,[0 for i in letter])
            
    def sparks(self,x,y,radius=15):
        
        for i in range (0,(2*math.pi) * random.randint(1,5)):
            cx = int(x + 4 + random.randint(1,radius) * math.cos(i) ) 
            cy = int(y + 4 + random.randint(1,radius) * math.sin(i))
            self.plotPixel(cx,cy)
            utime.sleep_us(100)
            
            self.pixelsToClear += [[cx,cy]]
        
        
    

display = [SSD(),SSD(port=1,scl=19,sda=18)]


display[0].address = 61
display[1].address = 60

for i in display:
    i.powerUp()
    i.clear(on =0)
pixelObject = [0b00000000,
        0b10011001,
        0b01111110,
        0b01111110,
        0b11111111,
        0b01111110,
        0b01111110,
        0b10011001]

x = 30
y = 30

incx = 1
incy = 1

pixelsToClear = []

effectDelay = 0;
while True:
    effectDelay += 1
    for d in display:
        if len(d.pixelsToClear) > 0:
            pixel = d.pixelsToClear[0]
            d.plotPixel(pixel[0],pixel[1],on=0)
            d.pixelsToClear.pop(0)
            
    #    d.plotPixel(x,y,on=0)
        d.writeObject(x,y,pixelObject,on=0)
    x += incx
    y += incy
    
    if x < 10 or x > 120:
        incx *= -1
        [d.sparks(x,y,7) for d in display]
  
            
            
    if y < 10 or y > 40:
        incy *= -1
        [d.sparks(x,y,7) for d in display]
    
    #[ d.plotPixel(x,y) for d in display]
    [ d.writeObject(x,y,pixelObject) for d in display]
    
    utime.sleep(0.002)
    

