/*  LS7366R_Encoder_Count.ino

    copyright Vulintus, Inc., 2024

    Example demonstrating communication with the LS7366R quadrature decoder.

    UPDATE LOG:
      2024-01-15 - Drew Sloan - Example first created.

*/


// Included libraries. //
#include <Vulintus_LS7366R.h>           // Vulintus LS7366R library.

// Serial communication constants. //
#define SERIAL_BAUD_RATE  115200        //Serial baud rate.

// Pin definitions. //
#define PIN_LS7366_CS   4

// LS7366R quadrature decoder. //
// Vulintus_LS7366R encoder = Vulintus_LS7366R(PIN_LS7366_CS);             // << Call this if using the default SPI bus.
Vulintus_LS7366R encoder = Vulintus_LS7366R(&SPI, PIN_LS7366_CS);   // << Call this if using a specified SPI bus.
int32_t count;                          //Current encoder count.


// INITIALIZATION ************************************************************//
void setup() {

  //Initialize serial data transmission.
  Serial.begin(SERIAL_BAUD_RATE);        

  //Print the filename, date, and time to verify the upload.         
  Print_FW_Filename();
  Print_FW_Date(); 
  Print_FW_Time(); 
  Serial.println();

  // Initialize the LS7466R.
  encoder.begin();
  encoder.clear_count();

}


// MAIN LOOP *****************************************************************//
void loop() {

  // Use the Arduino IDE Serial Plotter to best see the signal.
  count = encoder.read_count();
  Serial.println(count);
  delay(100);
  
}


// SUBFUNCTIONS **************************************************************//

//Print the sketch filename from the macro.
void Print_FW_Filename(void) {
  char s[] = __FILE__;                  //Grab the filename from the macro.
  uint8_t b = sizeof(s);                //Find the number of characters in the filename.
  while ((b > 0) && (s[b] != '\\')) {   //Loop until a forward slash is found.
    b--;                                //Step backward through the filename.
  }   
  char *u = &s[++b];                    //Create a pointer to start at the filename.
  Serial.println(u);                    //Print the filename.
}


//Print the sketch upload date from the macro.
void Print_FW_Date(void) {
  char s[] = __DATE__;                  //Grab the upload date from the macro.
  Serial.println(s);                    //Print the upload date.
}


//Print the sketch upload time from the macro.
void Print_FW_Time(void) {
  char s[] = __TIME__;                  //Grab the upload time from the macro.
  Serial.println(s);                    //Print the upload date.
}