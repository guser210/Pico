#include "hardware/i2c.h"
#include "hardware/flash.h"

extern "C" {
    void flash_settings(uint32_t flash_offs, const uint8_t *data, size_t count);
}
class BNO055
{
private: // var.
    i2c_inst *port;
    uint8_t address;
    uint16_t sda;
    uint16_t scl;

    uint8_t value;
    uint8_t data[22];

    int32_t flash_offset = (512 * 1024);
    const uint8_t *flash_contents = (const uint8_t *) (XIP_BASE + flash_offset);

    uint8_t memorySettings[32];
    uint8_t sizeOfSettings = sizeof(memorySettings);
public: //var.
    int heading;
    int roll;
    int rollDec;
    int pitch;

    uint8_t calibrate = 0;
    uint8_t calibrationData = 0;

    uint8_t calMag;
    uint8_t calAcc;
    uint8_t calGyr;
    uint8_t calSys;
    

public: // funcs.
    BNO055(i2c_inst *port, uint8_t address, uint16_t sda, uint16_t scl);

    void readReg(uint8_t reg, uint8_t *data, uint8_t sizeOfdata = 1);
    void writeReg(uint8_t reg, uint8_t *data, uint8_t sizeOfdata = 1);

    void enableConfig();
    void enableNDOF();

    void reset();

    void getData();

    void readMemory(uint8_t *data, int size, int location = 0);
    void writeMemory(uint8_t *data, int size, int location = 0);

private:
    /* data */
public:
    BNO055(/* args */);
    ~BNO055();
};

BNO055::BNO055(/* args */)
{
}

BNO055::~BNO055()
{
}

BNO055::BNO055(i2c_inst *port, uint8_t address, uint16_t sda, uint16_t scl)
{
    this->port = port;
    this->address = address;
    this->sda = sda;
    this->scl = scl;

    i2c_init(port, 100000);

    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);

    gpio_pull_up(sda);
    gpio_pull_up(scl);

    gpio_init(25);
    gpio_set_dir(25, 1);

    reset();
    enableNDOF();
}

void BNO055::readReg(uint8_t reg, uint8_t *data, uint8_t sizeOfdata)
{
    i2c_write_blocking(port, address, &reg, sizeOfdata, true);
    i2c_read_blocking(port, address, data, sizeOfdata, false);
}
void BNO055::writeReg(uint8_t reg, uint8_t *data, uint8_t sizeOfdata)
{
    uint8_t buffer[sizeOfdata + 1]{reg};

    memcpy(buffer + 1, data, sizeOfdata);

    i2c_write_blocking(port, address, buffer, sizeOfdata + 1, false);
}

void BNO055::enableConfig(){
    value = 0;
    writeReg(0x3d, &value);
    sleep_ms(19);
}
void BNO055::enableNDOF(){

    gpio_put(25, 1);
    readMemory(&value, 1, sizeOfSettings - 24);
    if( value == 1 && calibrate == 0){
        readMemory(data,2, sizeOfSettings - 23);
        enableConfig();
        writeReg(0x55, data, 22);
        gpio_put(25, 0);
    }

    value = 0b11000000;
    writeReg(0x3f, &value);

    value = 0b00001100;
    writeReg(0x3d, &value);
    sleep_ms(7);
}

void BNO055::reset(){
    enableConfig();

    value = 0b00100000;
    writeReg(0x3f, &value);
    sleep_ms(600);
}

void BNO055::getData(){
    readReg(0x1a, data, 6);

    heading = (int32_t)(((int8_t)data[1])<<8 | data[0]);
    heading /= 16;
    roll = ((int32_t)(((int8_t)data[3])<<8 | data[2])) *100;
    roll /= 16;
    rollDec = abs( roll % 100);
    roll /= 100;

    pitch = (int32_t)(((int8_t)data[5])<<8 | data[4]);
    pitch /= 16;

    readReg(0x35, &calibrationData);

    calMag = (0b00000011 & calibrationData);
    calAcc = (0b00001100 & calibrationData)>>2;
    calGyr = (0b00110000 & calibrationData)>>4;
    calSys = (0b11000000 & calibrationData)>>6;


    if( calibrate == 1){
        calibrate = 2;
        reset();
        enableNDOF();

    }else if( calibrate == 2 && calibrationData == 0xff){
        calibrate = 0;

        enableConfig();
        readReg(0x55, data, 22);
        writeMemory(data, 22, sizeOfSettings - 23);
        value = 1;
        writeMemory(&value, 1, sizeOfSettings - 24);
        enableNDOF();

    }

}
    void BNO055::readMemory(uint8_t *data, int size, int location){
        size = size > sizeOfSettings ? sizeOfSettings : size;
        memcpy(data,flash_contents + location, size);

    }
    void BNO055::writeMemory(uint8_t *data, int size, int location){
        size = size > sizeOfSettings ? sizeOfSettings : size;
        readMemory(memorySettings, sizeOfSettings);

        for( int index = 0; index < size; index++){
            location = location >= sizeOfSettings ? sizeOfSettings : location;

            memorySettings[location++] = data[index];
        }

        uint8_t buffer[ FLASH_PAGE_SIZE] = {0};
        memcpy(buffer, memorySettings,sizeOfSettings);
        flash_settings(flash_offset, buffer, FLASH_PAGE_SIZE);


    }
