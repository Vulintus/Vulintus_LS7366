/*  Vulintus_LS7366R.h - copyright Vulintus, Inc., 2024

    Vulintus STMicroelectronics DRV8434S eCompass module library.

    UPDATE LOG:
      2022-05-25 - Drew Sloan - Library first created.
      2024-01-15 - Drew Sloan - Made SPI settings explicit.

*/


#include "Vulintus_LS7366R.h" 					    //Make sure to include the corresponding header file.


//Class constructor (default SPI with chip-select).
Vulintus_LS7366R::Vulintus_LS7366R(uint8_t cs_pin)
        : _cs_pin(cs_pin)
{
    _spi_bus = &SPI;                    //Use the default SPI bus.
}


//Class constructor (specified SPI with chip-select).
Vulintus_LS7366R::Vulintus_LS7366R(SPIClass *spi_bus, uint8_t cs_pin)
        : _cs_pin(cs_pin)
{
    _spi_bus = spi_bus;                 //Use the specified SPI bus.
}


//Initialization function.
uint8_t Vulintus_LS7366R::begin(void)
{
    pinMode(_cs_pin,OUTPUT);            //Set the chip select pin mode to output.
    digitalWrite(_cs_pin,HIGH);         //Set the chip select pin high.
    SPI.begin();                        //Initialize SPI communication.
    _counter_size = 4;                  //Set the default counter size to 4 bytes.
    _mdr0_val = 0;                      //Reset the mode register 0 value.
    _mdr0_val |= LS7366R_QUADRX4;        //4x quadrature count mode (four counts per quadrature cycle).
    _mdr0_val |= LS7366R_FREE_RUN;       //Free-running count mode.
    _mdr0_val |= LS7366R_DISABLE_IDX;    //Disable index.
    _mdr0_val |= LS7366R_ASYNC_IDX;      //Asynchronous Index.
    _mdr0_val |= LS7366R_FILTER_1;       //Filter clock division factor = 1.
    Vulintus_LS7366R::write_register(LS7366R_SEL_MDR0,_mdr0_val);    //Set mode register 0.
    _mdr1_val = 0;                       //Reset the mode register 0 value.
    _mdr1_val |= LS7366R_BYTE_4;         //4-byte counter mode.
    _mdr1_val |= LS7366R_EN_CNTR;        //Enable counting.
    _mdr1_val |= LS7366R_CY_FLAG;        //FLAG on CY (B7 of STR).
    Vulintus_LS7366R::write_register(LS7366R_SEL_MDR1,_mdr1_val);    //Set mode register 1.
}


//Read the status register.
uint8_t Vulintus_LS7366R::status()
{
    uint8_t status = Vulintus_LS7366R::read_1byte_register(LS7366R_SEL_STR);    //Call the private function to read the register.
	return status;                                                              //Return the status.
}        


//Clear the status register.
void Vulintus_LS7366R::clear_status(void)
{
    clear_register(LS7366R_SEL_STR);    //Call the private function to clear the register.
}


//Read the counter register.
 int32_t Vulintus_LS7366R::read_count()
 {
    int32_t count = Vulintus_LS7366R::read_4byte_register(LS7366R_SEL_CNTR);    //Call the private function to read the register.
	return count;                                                               //Return the status.
 }              


//Clear the counter register. 
void Vulintus_LS7366R::clear_count()
{
    Vulintus_LS7366R::clear_register(LS7366R_SEL_CNTR);   //Call the private function to clear the register.
}                                    


//Write to the data register.
void Vulintus_LS7366R::write_DTR(uint32_t val)
{
    uint8_t spi_byte = LS7366R_SEL_DTR | LS7366R_WRITE_REG;         //Create a byte to hold the SPI command.
    spi_start();                                                    //Start SPI transmission.
    SPI.transfer(spi_byte);                                         //Send the command to write to the data register.
    for (uint8_t i = 0; i < _counter_size; i++) {                   //Step through the bytes of the counter.
        spi_byte = (uint8_t)(val >> (8*(_counter_size - 1 - i)));   //Bitshift and grab each byte of the value.
        SPI.transfer(spi_byte);                                     //Send each byte.
    }
    spi_end();                                                      //End SPI transmission.
}      


//Copy the data register to the counter register.
void Vulintus_LS7366R::load_DTR()
{
    spi_start();                                        //Start SPI transmission.
	SPI.transfer(LS7366R_SEL_DTR | LS7366R_LOAD_REG);   //Send the command to load the data register.
	spi_end();                                          //End SPI transmission.    
}       
          

//Copy the counter register to the output register.
void Vulintus_LS7366R::load_OTR()
{
	spi_start();                                        //Start SPI transmission.
	SPI.transfer(LS7366R_SEL_OTR | LS7366R_LOAD_REG);   //Send the command to load the output register.
	spi_end();                                          //End SPI transmission.
}      


//Read from the output register.
int32_t Vulintus_LS7366R::read_OTR()
{
    int32_t count = Vulintus_LS7366R::read_4byte_register(LS7366R_SEL_OTR);     //Call the private function to read the register.
	return count;                                                               //Return the status.
}      
            

//Set the counter size.
void Vulintus_LS7366R::set_counter_size(uint8_t count_bytes)
{
    _counter_size = count_bytes;        //Copy the specified counter size to the private variable.
    _mdr1_val &= 0xFC;                  //Clear the two bottom bytes of the mode register 0 value.
    count_bytes = (4 - count_bytes);    //Flip the counter size to match the register setting.
    _mdr1_val |= count_bytes;           //Set the counter size in the register value.
    Vulintus_LS7366R::write_register(LS7366R_SEL_MDR1,_mdr1_val);    //Set mode register 1.
}


//Set the count mode.
void Vulintus_LS7366R::set_count_mode(uint8_t count_mode)
{
    _mdr0_val &= 0xFC;                  //Clear the two bottom bytes of the mode register 0 value.
    _mdr0_val |= count_mode;            //Set the counter size in the register value.
    Vulintus_LS7366R::write_register(LS7366R_SEL_MDR0,_mdr0_val);    //Set mode register 0.
} 


//Clear the specified register.
void Vulintus_LS7366R::clear_register(uint8_t reg_id)
{
    uint8_t spi_byte = reg_id | LS7366R_CLR_REG;    //Create a byte to hold the SPI command.
    spi_start();                                    //Start SPI transmission.
	SPI.transfer(spi_byte);                         //Send the command to clear the status register.
	spi_end();                                      //End SPI transmission.
}


//Write to the specified register.
void Vulintus_LS7366R::write_register(uint8_t reg_id, uint8_t val)
{
    uint8_t spi_byte = reg_id | LS7366R_WRITE_REG;  //Create a byte to hold the SPI command.
    spi_start();                                    //Start SPI transmission.
	SPI.transfer(spi_byte);                         //Send the command to clear the status register.
    SPI.transfer(val);                              //Send the specified value.
	spi_end();                                      //End SPI transmission.
}


//Read the specified 1-byte register.
uint8_t Vulintus_LS7366R::read_1byte_register(uint8_t reg_id)
{
    uint8_t spi_byte = reg_id | LS7366R_READ_REG;   //Create a byte to hold the SPI command.
    spi_start();                                    //Start SPI transmission.
	SPI.transfer(spi_byte);                         //Send the command to clear the status register.
    spi_byte = SPI.transfer(0x00);                  //Send a dummy byteVulintus_LS7366R to receve the reply.
	spi_end();                                      //End SPI transmission.
    return spi_byte;                                //Return the status.
}


//Read the specified 4-byte register.
int32_t Vulintus_LS7366R::read_4byte_register(uint8_t reg_id)
{
     int32_t reg_val;                               //Create a 32-bit integer to hold the register value.
     uint8_t spi_byte = reg_id | LS7366R_READ_REG;  //Create a byte to hold the SPI command/reply.
     spi_start();                                   //Start SPI transmission.
     SPI.transfer(spi_byte);                        //Send the command to read from the specified register.
     for (uint8_t i = 0; i < _counter_size; i++) {  //Step through the bytes of the counter.
        spi_byte = SPI.transfer(0x00);              //Send a dummy byte to receve the reply.
        reg_val = (reg_val << 8) + spi_byte;        //Shift the previous bytes and add the new one.
     }
    spi_end();                                      //End SPI transmission.
	return reg_val;                                 //Return the count.
}


// Start an SPI transaction.
void Vulintus_LS7366R::spi_start(void)
{
  digitalWrite(_cs_pin, LOW);     // Set the chip select line low.
  _spi_bus->beginTransaction(SPISettings(LS7366R_SPI_SPEED, MSBFIRST, LS7366R_SPI_MODE));   // Set the SPI settings for this chip.  
  delayMicroseconds(1);           // Pause for 1 microsecond. 
}


// End an SPI transaction.
void Vulintus_LS7366R::spi_end(void)
{
  _spi_bus->endTransaction();     // Release the SPI bus.
  digitalWrite(_cs_pin, HIGH);    // Set the chip select line high. 
}