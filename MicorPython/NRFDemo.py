
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
        self.writeReg(1,0b00000011) #no ack
        
    
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
        
        self.spi.write(bytearray(localData))
        
        self.csnHigh()
        
        self.ceHigh()
        utime.sleep_us(10)
        for i in range(0,10000):
            reg = self.readReg(7)[0]
            if reg & 0b00110000:
                break
        self.ceLow()
        self.writeReg(7,0b00110000) # Clear status flags.
        
        
        
    def readMessage(self,size=32):
        reg = [0b01100001]
        
        self.csnLow()
        self.spi.write(bytearray(reg))
        result = self.spi.read(size)
        self.csnHigh()
        self.writeReg(0x07,0b01000000) # clear status flags
        return result
        
    def newMessage(self):
        regfs = self.readReg(0x17)[0]
        regs = self.readReg(7)[0]
        return  (not (0b00000001 & regfs)) or (0b01000000 & regs) 
    
    
nrf = NRF()
nrf.config()
nrf.modeRX()

messageToSend = "Data"

timeout = 0;
counter = 0
while True:


    if nrf.newMessage(): # print any incoming message
        print("recv: ","".join([chr(i) for i in nrf.readMessage()]))

   
    # if you want to send a message you do this.
    timeout += 1
    if timeout >= 1000 and messageToSend != "":
        timeout= 0
        counter += 1
        nrf.modeTX() # change to transmitter.
        nrf.sendMessage(messageToSend + str(counter)) # Send message
        nrf.modeRX() # change to receiver.
        
    
    


