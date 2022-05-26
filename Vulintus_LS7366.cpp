#include "Vulintus_LS7366.h" 					    //Make sure to include the corresponding header file.


//Class constructor.
Vulintus_LS7366::Vulintus_LS7366(byte cs_pin)
{
    _cs_pin = cs_pin;                   //Copy the chip select pin to the private variable.
}


//Initialization function.
uint8_t Vulintus_LS7366::begin(void)
{
    SPI.begin();                        //Initialize SPI communication.
    SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV8, MSBFIRST, SPI_MODE0)); //SPI settings (clock = 2 MHz, most-significant bit first, SPI mode = 0).
    pinMode(_cs_pin,OUTPUT);            //Set the chip select pin mode to output.
    digitalWrite(_cs_pin,HIGH);         //Set the chip select pin high.
    _counter_size = 2;                  //Set the default counter size to 2 bytes.
    _mdr0_val = 0;                      //Reset the mode register 0 value.
    _mdr0_val |= LS7366_QUADRX4;        //4x quadrature count mode (four counts per quadrature cycle).
    _mdr0_val |= LS7366_FREE_RUN;       //Free-running count mode.
    _mdr0_val |= LS7366_DISABLE_IDX;    //Disable index.
    _mdr0_val |= LS7366_ASYNC_IDX;      //Asynchronous Index.
    _mdr0_val |= LS7366_FILTER_1;       //Filter clock division factor = 1.
    Vulintus_LS7366::write_register(LS7366_SEL_MDR0,_mdr0_val);    //Set mode register 0.
    _mdr1_val = 0;                      //Reset the mode register 0 value.
    _mdr1_val |= LS7366_BYTE_2;         //2-byte counter mode.
    _mdr1_val |= LS7366_EN_CNTR;        //Enable counting.
    _mdr1_val |= LS7366_CY_FLAG;        //FLAG on CY (B7 of STR).
    Vulintus_LS7366::write_register(LS7366_SEL_MDR1,_mdr1_val);    //Set mode register 1.
}


//Read the status register.
uint8_t Vulintus_LS7366::status()
{
    uint8_t status = Vulintus_LS7366::read_1byte_register(LS7366_SEL_STR);  //Call the private function to read the register.
	return status;                                                          //Return the status.
}        


//Read the counter register.
 int32_t Vulintus_LS7366::read_count()
 {
    int32_t count = Vulintus_LS7366::read_4byte_register(LS7366_SEL_CNTR);  //Call the private function to read the register.
	return count;                                                           //Return the status.
 }              


//Clear the counter register. 
void Vulintus_LS7366::clear_count()
{
    Vulintus_LS7366::clear_register(LS7366_SEL_CNTR);   //Call the private function to clear the register.
}                                    


//Write to the data register.
void Vulintus_LS7366::write_DTR(uint32_t val)
{
    uint8_t spi_byte = LS7366_SEL_DTR | LS7366_WRITE_REG;           //Create a byte to hold the SPI command.
    digitalWrite(_cs_pin, LOW);                                     //Set the chip select line LOW.
     SPI.transfer(spi_byte);                                        //Send the command to write to the data register.
     for (uint8_t i = 0; i < _counter_size; i++) {                  //Step through the bytes of the counter.
        spi_byte = (uint8_t)(val >> (8*(_counter_size - 1 - i)));   //Bitshift and grab each byte of the value.
        SPI.transfer(spi_byte);                                     //Send each byte.
     }
    digitalWrite(_cs_pin, HIGH);                         //Set the chip select line HIGH.
}      


//Copy the data register to the counter register.
void Vulintus_LS7366::load_DTR()
{
    digitalWrite(_cs_pin, LOW);                         //Set the chip select line LOW.
	SPI.transfer(LS7366_SEL_DTR | LS7366_LOAD_REG);     //Send the command to load the data register.
	digitalWrite(_cs_pin, HIGH);                        //Set the chip select line HIGH.    
}       
          

//Copy the counter register to the output register.
void Vulintus_LS7366::load_OTR()
{
	digitalWrite(_cs_pin, LOW);                         //Set the chip select line LOW.
	SPI.transfer(LS7366_SEL_OTR | LS7366_LOAD_REG);     //Send the command to load the output register.
	digitalWrite(_cs_pin, HIGH);                        //Set the chip select line HIGH.
}      


//Read from the output register.
int32_t Vulintus_LS7366::read_OTR()
{
    int32_t count = Vulintus_LS7366::read_4byte_register(LS7366_SEL_CNTR);  //Call the private function to read the register.
	return count;                                                           //Return the status.
}      
            

//Set the counter size.
void Vulintus_LS7366::set_counter_size(uint8_t count_bytes)
{
    _counter_size = count_bytes;        //Copy the specified counter size to the private variable.
    _mdr1_val &= 0xFC;                  //Clear the two bottom bytes of the mode register 0 value.
    count_bytes = (4 - count_bytes);    //Flip the counter size to match the register setting.
    _mdr1_val |= count_bytes;           //Set the counter size in the register value.
    Vulintus_LS7366::write_register(LS7366_SEL_MDR1,_mdr1_val);    //Set mode register 1.
}


//Set the count mode.
void Vulintus_LS7366::set_count_mode(uint8_t count_mode)
{
    _mdr0_val &= 0xFC;                  //Clear the two bottom bytes of the mode register 0 value.
    _mdr1_val |= count_mode;            //Set the counter size in the register value.
    Vulintus_LS7366::write_register(LS7366_SEL_MDR0,_mdr0_val);    //Set mode register 0.
} 


//Clear the specified register.
void Vulintus_LS7366::clear_register(uint8_t reg_id)
{
    uint8_t spi_byte = reg_id | LS7366_CLR_REG;         //Create a byte to hold the SPI command.
    digitalWrite(_cs_pin, LOW);                         //Set the chip select line LOW.
	SPI.transfer(spi_byte);                             //Send the command to clear the status register.
	digitalWrite(_cs_pin, HIGH);                        //Set the chip select line HIGH.
}


//Write to the specified register.
void Vulintus_LS7366::write_register(uint8_t reg_id, uint8_t val)
{
    uint8_t spi_byte = reg_id | LS7366_WRITE_REG;       //Create a byte to hold the SPI command.
    digitalWrite(_cs_pin, LOW);                         //Set the chip select line LOW.
	SPI.transfer(spi_byte);                             //Send the command to clear the status register.
    SPI.transfer(val);                                  //Send the specified value.
	digitalWrite(_cs_pin, HIGH);                        //Set the chip select line HIGH.
}


//Read the specified 1-byte register.
uint8_t Vulintus_LS7366::read_1byte_register(uint8_t reg_id)
{
    uint8_t spi_byte = reg_id | LS7366_READ_REG;        //Create a byte to hold the SPI command.
    digitalWrite(_cs_pin, LOW);                         //Set the chip select line LOW.
	SPI.transfer(spi_byte);                             //Send the command to clear the status register.
    spi_byte = SPI.transfer(0x00);                      //Send a dummy byte to receve the reply.
	digitalWrite(_cs_pin, HIGH);                        //Set the chip select line HIGH.
    return spi_byte;                                    //Return the status.
}


//Read the specified 4-byte register.
int32_t Vulintus_LS7366::read_4byte_register(uint8_t reg_id)
{
     int32_t reg_val;                                   //Create a 32-bit integer to hold the register value.
     uint8_t spi_byte = reg_id | LS7366_READ_REG;       //Create a byte to hold the SPI command/reply.
     digitalWrite(_cs_pin, LOW);                        //Set the chip select line LOW.
     SPI.transfer(spi_byte);                            //Send the command to read from the specified register.
     for (uint8_t i = 0; i < _counter_size; i++) {      //Step through the bytes of the counter.
        spi_byte = SPI.transfer(0x00);                  //Send a dummy byte to receve the reply.
        reg_val = (reg_val << 8) + spi_byte;            //Shift the previous bytes and add the new one.
     }
    digitalWrite(_cs_pin, HIGH);                        //Set the chip select line HIGH.
	return reg_val;                                     //Return the count.
}