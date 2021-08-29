import utime
from machine import Pin
from machine import I2C


class SSD:
    def __init__(self, port=0, freq=400000, scl=17, sda=16):
        self.address = 60
        self.dev = I2C(port, freq=freq,scl=Pin(scl),sda=Pin(sda))
        
    def printDevices(self):
        devices = self.dev.scan()
        for device in devices:
            print(device)
            
    def sendCommand(self, cmd, data):
        command = [0b11000000 & cmd, data]
        self.dev.writeto(self.address, bytearray(command))
        
    def powerUp(self):
        
        self.sendCommand(0b10000000, 0xa4)
        self.sendCommand(0b00000000, 0xaf)
    
    def clear(self, on=255):
        for page in range(0xB0, 0xb8):
            self.sendCommand(0b10000000, page)
            self.sendCommand(0b00000000, 0x00)
            self.sendCommand(0b00000000, 0x10)
            
            self.dev.writeto(self.address, bytearray([0b01000000] + [on for i in range(0,129)]))
    def plotPixel(self, x=10, y=10, on=1):
        
        xl = int(x & 0x0f)
        xh = int((x>>4 & 0x0f) | 0b00010000)
        
        pixel = on<<(y%7)
        
        page = int(( y - (y%y))/7) | 0xb0
        
        self.sendCommand(0b10000000, page)
        self.sendCommand(0b00000000, xl)
        self.sendCommand(0b00000000, xh)
        
        self.sendCommand(0b01000000,pixel)
                
    
display = SSD()

display.printDevices()
display.powerUp()
display.clear(on=0)

display.plotPixel(64,32)


x = 30
y = 30

incx = 1
incy = 1


while True:
    
    display.plotPixel(x,y,on = 0)
    x += incx
    y += incy
    
    if x < 5 or x > 120: incx *= -1
    if y < 5 or y > 50: incy *= -1
    
    display.plotPixel(x,y)
    
    utime.sleep(0.005)
    








