
class NRF24L01
{

private: // vars.
    spi_inst_t *spiPort;
    uint16_t csnPin;
    uint16_t cePin;


public: //funcs.

    void csnLow(){ gpio_put(csnPin, 0);}
    void csnHigh(){ gpio_put(csnPin, 1);}
    void ceLow(){ gpio_put(cePin, 0);}
    void ceHigh(){ gpio_put(cePin, 1);}

    NRF24L01(spi_inst_t* spiPort, uint16_t csnPin, uint16_t cePin);

    uint8_t readReg(uint8_t reg);

    void writeReg(uint8_t reg, uint8_t data);
    void writeReg(uint8_t reg, uint8_t *data, uint8_t dataSize);

    void config();

    void modeTX();
    void modeRX();

    void sendMessage(char *msg);
    void receiveMessage(char* msg);

    uint8_t newMessage();


private:
    /* data */
public:
    NRF24L01(/* args */);
    ~NRF24L01();
};

NRF24L01::NRF24L01(/* args */)
{
}

NRF24L01::~NRF24L01()
{
}

    NRF24L01::NRF24L01(spi_inst_t* spiPort, uint16_t csnPin, uint16_t cePin){
        this->spiPort = spiPort;
        this->csnPin = csnPin;
        this->cePin = cePin;

        gpio_init(csnPin);
        gpio_set_dir(csnPin, 1);
        gpio_init(cePin);
        gpio_set_dir(cePin, 1);

        spi_init(spiPort, 1000000);

        gpio_set_function(12, GPIO_FUNC_SPI);
        gpio_set_function(11, GPIO_FUNC_SPI);
        gpio_set_function(10, GPIO_FUNC_SPI);

        ceLow();
        csnHigh();

    }

    uint8_t NRF24L01::readReg(uint8_t reg){
        uint8_t result = 0;

        csnLow();

        spi_write_blocking(spiPort,&reg,1);
        spi_read_blocking(spiPort,0xff, &result, 1);

        csnHigh();

        return result;
    }

    void NRF24L01::writeReg(uint8_t reg, uint8_t data){
        writeReg(reg,&data,1);
    }
    void NRF24L01::writeReg(uint8_t reg, uint8_t *data, uint8_t dataSize){
        reg = 0b00100000 | (0b00011111 & reg);

        csnLow();
        spi_write_blocking(spiPort,&reg, 1);

        spi_write_blocking(spiPort, data, dataSize);

        csnHigh();

    }

    void NRF24L01::config(){
        ceLow();
        csnHigh();
        sleep_ms(11);


        writeReg(0, 0b00001010); // config
        sleep_ms(2);
        writeReg(1, 0); // no ack

        writeReg(5, 60); // channel.

        writeReg(0x0a, (uint8_t*)"gyroc",5);
        writeReg(0x10, (uint8_t*)"gyroc",5);


        writeReg(0x11, 32);





    }

    void NRF24L01::modeTX(){
            
            uint8_t config = readReg(0);

            config &= ~(1<<0);

            writeReg(0, config);

            ceLow();

            sleep_us(130);

    }
    void NRF24L01::modeRX(){
            uint8_t config = readReg(0);

            config |= (1<<0);

            writeReg(0, config);

            ceHigh();

            sleep_us(130);

    }

    void NRF24L01::sendMessage(char *msg){
        uint8_t reg = 0b10100000;

        csnLow();

        spi_write_blocking(spiPort,&reg, 1);

        spi_write_blocking(spiPort, (uint8_t*)msg, 32);

        csnHigh();

        ceHigh();
        sleep_us(10);

        ceLow();

        writeReg(7, 0b00110000);

    }
    void NRF24L01::receiveMessage(char* msg){
        uint8_t reg = 0b01100001;

        csnLow();

        spi_write_blocking(spiPort,&reg, 1);

        spi_read_blocking(spiPort,0xff,(uint8_t*)msg,32);

        csnHigh();

          writeReg(7, 0b01000000);


    }

    uint8_t NRF24L01::newMessage(){
        uint8_t fifo = readReg(0x17);

        return !(0b00000001 & fifo);
    }
