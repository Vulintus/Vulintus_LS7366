/*  Vulintus_LS7366R.h - copyright Vulintus, Inc., 2024

    Vulintus STMicroelectronics DRV8434S eCompass module library.

    UPDATE LOG:
      2022-05-25 - Drew Sloan - Library first created.
      2024-01-15 - Drew Sloan - Made SPI settings explicit.

*/


#ifndef VULINTUS_LS7366R_H
#define VULINTUS_LS7366R_H

#include <Arduino.h>						// Arduino main include file.
#include <SPI.h>							// Standard Arduino SPI library.

#define LS7366R_SPI_SPEED	5000000		    // SPI clock speed, in Hz.
#define LS7366R_SPI_MODE	SPI_MODE0		// SPI mode (data capture on rising edge).


// STATUS REGISTER (STR) ****************************************************************************************// 
    #define LS7366R_STATUS_S         0x01         // Sign bit (1 = negative, 0 = positive).
    #define LS7366R_STATUS_UD        0x02         // Count direction indicator (0 = count down, 1 = count up).
    #define LS7366R_STATUS_PLS       0x04         // Power loss indicator latch; set upon power up.
    #define LS7366R_STATUS_CEN       0x08         // Count enable status (0 = counting disabled, 1 = counting enabled).
    #define LS7366R_STATUS_IDX       0x10         // Index latch.
    #define LS7366R_STATUS_CMP       0x20         // Compare (CNTR = DTR) latch.
    #define LS7366R_STATUS_BW        0x40         // Borrow (CNTR underflow) latch.
    #define LS7366R_STATUS_CY        0x80         // Carry (CNTR overflow) latch.

// INSTRUCTION REGISTER (IR) ************************************************************************************// 
    #define LS7366R_SEL_MDR0         0x08         // Select MDR0.
    #define LS7366R_SEL_MDR1         0x10         // Select MDR1.
    #define LS7366R_SEL_DTR          0x18         // Select DTR.
    #define LS7366R_SEL_CNTR         0x20         // Select CNTR.
    #define LS7366R_SEL_OTR          0x28         // Select OTR.
    #define LS7366R_SEL_STR          0x30         // Select STR.
    #define LS7366R_CLR_REG          0x00         // Clear register.
    #define LS7366R_READ_REG         0x40         // Read register.
    #define LS7366R_WRITE_REG        0x80         // Write register.
    #define LS7366R_LOAD_REG         0xC0         // Load register.

// MODE REGISTER 0 (MDR0) ***************************************************************************************// 
    #define LS7366R_NONQUAD          0x00         // Non-quadrature count mode. (A = clock, B = direction).
    #define LS7366R_QUADRX1          0x01         // 1x quadrature count mode (one count per quadrature cycle).
    #define LS7366R_QUADRX2          0x02         // 2x quadrature count mode (two counts per quadrature cycle).
    #define LS7366R_QUADRX4          0x03         // 4x quadrature count mode (four counts per quadrature cycle).
    #define LS7366R_FREE_RUN         0x00         // Free-running count mode.
    #define LS7366R_SINGLE_CYCLE     0x04         // Single-cycle count mode (counter disabled with carry or borrow, re-enabled with reset or load)
    #define LS7366R_RANGE_LIMIT      0x08         // Range-limit count mode (up and down count-ranges are limited between DTR and zero, respectively; counting freezes at these limits but resumes when direction reverses).
    #define LS7366R_MODULO_N         0x0C         // Modulo-n count mode (input count clock frequency is divided by a factor of (n+1), where n = DTR, in both up and down directions).
    #define LS7366R_DISABLE_IDX      0x00         // Disable index.
    #define LS7366R_IDX_LOADC        0x10         // Configure index as the "load CNTR" input (transfers DTR to CNTR).
    #define LS7366R_IDX_RESETC       0x20         // Configure index as the "reset CNTR" input (clears CNTR to 0). 
    #define LS7366R_IDX_LOADO        0x30         // Configure index as the "load OTR" input (transfers CNTR to OTR).
    #define LS7366R_ASYNC_IDX        0x00         // Asynchronous Index.
    #define LS7366R_SYNC_IDX         0x40         // Synchronous Index (overridden in non-quadrature mode).
    #define LS7366R_FILTER_1         0x00         // Filter clock division factor = 1
    #define LS7366R_FILTER_2         0x80         // Filter clock division factor = 2

// MODE REGISTER 1 (MDR1) ***************************************************************************************// 
    #define LS7366R_BYTE_4           0x00         // 4-byte counter mode.
    #define LS7366R_BYTE_3           0x01         // 3-byte counter mode. 
    #define LS7366R_BYTE_2           0x02         // 2-byte counter mode.
    #define LS7366R_BYTE_1           0x03         // 1-byte counter mode.
    #define LS7366R_EN_CNTR          0x00         // Enable counting.
    #define LS7366R_DIS_CNTR         0x04         // Disable counting.
    #define LS7366R_NO_FLAGS         0x00         // No flags enabled.
    #define LS7366R_IDX_FLAG         0x10         // FLAG on IDX (B4 of STR). 
    #define LS7366R_CMP_FLAG         0x20         // FLAG on CMP (B5 of STR). 
    #define LS7366R_BW_FLAG          0x40         // FLAG on BW (B6 of STR).
    #define LS7366R_CY_FLAG          0x80         // FLAG on CY (B7 of STR).
    

// CLASSES ******************************************************************************************************// 
class Vulintus_LS7366R {
	public:

		// Constructors.// 
		Vulintus_LS7366R(uint8_t cs_pin);                       // Default SPI with chip-select.
        Vulintus_LS7366R(SPIClass *spi_bus, uint8_t cs_pin);    // Specified SPI with chip-select.
		
		// Functions.// 
        uint8_t begin(void); 				            // Initialization function.

        uint8_t status(void);                           // Read the status register.     
        void clear_status(void);                        // Clear the status register.

        int32_t read_count(void);                       // Read the counter register.
        void clear_count(void);                         // Clear the counter register.       

        void write_DTR(uint32_t val);                   // Write to the data register.
        void load_DTR(void);                            // Copy the data register to the counter register.

        void load_OTR(void);                            // Copy the counter register to the output register.
        int32_t read_OTR(void);                         // Read the output register.                 

        void set_counter_size(uint8_t count_bytes);     // Set the counter size.
        void set_count_mode(uint8_t count_mode);        // Set the count mode.

	private:
		
        // Variables.// 
        SPIClass *_spi_bus = NULL;				        // SPI interface pointer.
        uint8_t _cs_pin;                                // Chip select pin.
        uint8_t _counter_size;                          // Counter size, in bytes (1, 2, or 4).
        uint8_t _mdr0_val;                              // Current value of mode register 0.
        uint8_t _mdr1_val;                              // Current value of mode register 1.

		// Functions.// 
        void clear_register(uint8_t reg_id);                // Clear the specified register.
        void write_register(uint8_t reg_id, uint8_t val);   // Write to the specified register.
        uint8_t read_1byte_register(uint8_t reg_id);        // Read the specified 1-byte register.
        int32_t read_4byte_register(uint8_t reg_id);        // Read the specified 4-byte register.
        void spi_start(void);					            // Start an SPI transaction.
		void spi_end(void);						            // End an SPI transaction.
};

#endif 								// #ifndef VULINTUS_LS7366R_H