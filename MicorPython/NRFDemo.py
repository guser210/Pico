
import utime
from machine import SPI
from machine import Pin

class NRF:
    def __init__(self, port=1, miso=12, mosi=11, sck=10, csn=9, ce = 8):
        self.csn = Pin(csn, 1)
        self.ce = Pin(ce,1)
        
        self.spi = SPI(port, 1000000, miso=Pin(miso), mosi=Pin(mosi), sck=Pin(sck))
        
        self.csnHigh()
        self.ceLow()
        
        utime.sleep_ms(11)
        
    def csnHigh(self): self.csn(1)
    def csnLow(self): self.csn(0)
    def ceHigh(self): self.ce(1)
    def ceLow(self): self.ce(0)
    
    def readReg(self, reg, size=1):
        reg = [0b00011111 & reg]
        self.csnLow()
        self.spi.write(bytearray(reg))
        
        result = self.spi.read(size)
        
        self.csnHigh()
        
        return result
    
    def writeReg(self, reg, data):
        reg = [0b00100000 | (0b00011111 & reg)]
        self.csnLow()
        self.spi.write(bytearray(reg))
        
        data = [data] if type(data) == type(1) else data
        self.spi.write(bytearray(data))
        
        self.csnHigh()
        
    def config(self, channel=60, channelName ="gyroc", packetSize = 32):
        self.csnHigh()
        self.ceLow()
        
        utime.sleep_ms(11)
        
        self.writeReg(0,0b00001010) # config
        utime.sleep_us(1500)
        self.writeReg(1,0b00000011)
        
        self.writeReg(4,0b00001111) # max retr
        
        self.writeReg(5, channel)
        
        self.writeReg(0x0a, channelName)
        self.writeReg(0x10, channelName)
        
        self.writeReg(0x11, packetSize) #
        
    def modeTX(self):
        reg = self.readReg(0)[0]
        reg &= ~(1<<0)
        self.writeReg(0,reg)
        self.ceLow()
        utime.sleep_us(130)
    def modeRX(self):
        reg = self.readReg(0)[0]
        reg |= (1<<0)
        self.writeReg(0,reg)
        self.ceHigh()
        utime.sleep_us(130)
    
        
    def sendMessage(self, data,size = 32):
        reg = [0b10100000]
        
        self.csnLow()
        self.spi.write(bytearray([0b11100001]))
        self.csnHigh()
        
        self.csnLow()
        self.spi.write(bytearray(reg))
        
        localData = bytearray(data)
        localData.extend(bytearray(size - len(localData)))
        
        print([len(localData),localData])
        self.spi.write(bytearray(localData))
        
        self.csnHigh()
        
        self.ceHigh()
        utime.sleep_us(10)
        self.ceLow()
        
        reg = self.readReg(7)[0]
        reg |= (1<<5)
        self.writeReg(7,reg)
        
        
    def readMessage(size=32):
        reg = [0b01100001]
        
        self.csnLow()
        self.spi.write(bytearray(reg))
        result = self.spi.read(size)
        self.csnHigh()
        
        return result
        
    

nrf = NRF()
nrf.config()

nrf.modeTX()

while True:
    a = nrf.readReg(0)[0]
    print(bin(a))
    a = nrf.readReg(7)
    print(["status" , bin(a[0])])
    
    nrf.sendMessage("MicroPython sucks")
    utime.sleep(0.1)
        
                           
