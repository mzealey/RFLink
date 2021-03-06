//#######################################################################################################
//##                    This Plugin is only for use with Nodo 3.7 Slaves (variablesend support)        ##
//##                                     Plugin-090 Nodo Slave                                         ##
//#######################################################################################################
// *********************************************************************************************
// * This Plugin takes care of reception of Nodo Slave 3.7
// *
// * Author             : StuntTeam
// * Support            : http://sourceforge.net/projects/rflink/
// * License            : This code is free for use in any open source project when this header is included.
// *                      Usage of any parts of this code in a commercial application is prohibited!
// *********************************************************************************************
// * Changelog: v1.0 initial release
// *********************************************************************************************
// ----------------------------------
// Variabele  5 : Temperatuur
// Variabele  6 : Luchtvochtigheid   0 - 100
// Variabele  7 : Rain
// Variabele  8 : Wind speed
// Variabele  9 : Wind direction     0 - 15
// Variabele 10 : Wind gust
// Variabele 11, 12 en 13 : Temperatuur
// Variabele 14 : Luchtvochtigheid   0 - 100
// Variabele 15 : Lichtsterkte       0 - 1024 dus mappen naar 1-100 
// Variabele 16 : Barometric pressure 
// ----------------------------------
// * Sample:
// * 20;9E;DEBUG;Pulses=194;Pulses(uSec)=3100,900,500,350,1475,375,525,300,1475,375,500,350,450,375,575,250,525,300,1475,375,1450,400,1450,400,425,425,425,425,1425,425,425,425,425,400,425,400,425,400,425,400,425,400,450,400,425,400,450,400,450,400,450,400,425,400,425,425,425,425,425,425,400,425,425,425,400,425,1425,425,400,425,400,425,400,425,400,425,400,425,425,425,400,425,425,425,425,425,1450,425,425,425,425,425,425,425,425,425,400,425,1425,425,1425,425,400,425,400,425,400,425,400,425,400,425,400,425,400,425,425,425,425,425,400,425,400,425,400,425,400,425,400,425,400,425,400,450,400,450,400,450,400,450,400,450,400,450,1400,450,400,450,400,450,400,425,400,450,400,425,1425,425,400,425,1425,425,1425,450,400,450,400,450,375,450,375,450,400,450,1425,450,400,450,1425,425,1425,450,400,450,400,425,400,450,400,450,400,450,400;
// * 20;9F;Slave;ID=0307;DEBUG=0014;
// * slave nodo 7, var 3 value 14

// 3100,900,
/*( 194-2=192/2 = 96
500,350,1475,375,525,300,1475,375,500,350,450,375,575,250,525,300,1475,375,1450,400,1450,400,425,425,425,425,1425,425,425,425,425,400,425,400,425,400,425,400,425,400,450,400,
425,400,450,400,450,400,450,400,425,400,425,425,425,425,425,425,400,425,425,425,400,425,1425,425,400,425,400,425,400,425,400,425,400,425,425,425,400,425,425,425,425,425,1450,
425,425,425,425,425,425,425,425,425,400,425,1425,425,1425,425,400,425,400,425,400,425,400,425,400,425,400,425,400,425,425,425,425,425,400,425,400,425,400,425,400,425,400,425,
400,425,400,450,400,450,400,450,400,450,400,450,400,450,1400,450,400,450,400,450,400,425,400,450,400,425,1425,425,400,425,1425,425,1425,450,400,450,400,450,375,450,375,450,
400,450,1425,450,400,450,1425,425,1425,450,400,450,400,425,400,450,400,450,400,450,400;

010100001110010000000
0000000000010000000001
000001100000000000000
000000010000010110000 
01011000000

01010000 11100100 00000000 00000000 10000000 00100000 11000000 00000000 00000001 00000101 10000010 11000000
00001010 00100111 00000000 00000000 00000001 00000100 00000011 00000000 10000000 10100000 01000001 00000011
50E400008020C0010582C0
A270000104030080A04103
A2^70^00^01^04^03^00^80^A0^41^03
A2^70^00^01^04^03^00^80^A0^41=
*/
// *********************************************************************************************
#define NODO_PULSE_MID              1000/RAWSIGNAL_SAMPLE_RATE  // PWM: Pulsen langer zijn '1'
#define NodoSlave_PULSECOUNT         194

#ifdef PLUGIN_090

struct DataBlockStruct {        // 16*sizeof(struct DataBlockStruct)+2 = 194   eg. 16*12+2
  byte Version;
  byte SourceUnit;              // event->SourceUnit=DataBlock.SourceUnit&0x1F;       // Maskeer de bits van het Home adres.
  byte DestinationUnit;         // event->DestinationUnit=DataBlock.DestinationUnit;  // destination nodo
  byte Flags;
  byte Type;                    // event->Type=DataBlock.Type;
  byte Command;                 // event->Command=DataBlock.Command;
  byte Par1;                    // event->Par1=DataBlock.Par1;
  unsigned long Par2;           // event->Par2=DataBlock.Par2;
  byte Checksum;
};  

boolean Plugin_090(byte function, char *string) {
      if (RawSignal.Number!=NodoSlave_PULSECOUNT ) return false; // Per byte twee posities + startbit.
      byte b,x,y,z;

      struct DataBlockStruct DataBlock;
      byte *B=(byte*)&DataBlock;                             // B wijst naar de eerste byte van de struct
      z=3;                                                   // RawSignal pulse teller: 0=niet gebruiktaantal, 1=startpuls, 2=space na startpuls, 3=1e pulslengte. Dus start loop met drie.

      for(x=0;x<sizeof(struct DataBlockStruct);x++) {        // vul alle bytes van de struct 
         b=0;
         for(y=0;y<=7;y++) {                                  // vul alle bits binnen een byte
            if((RawSignal.Pulses[z]) > NODO_PULSE_MID)      
              b|=1<<y;                                         // LSB in signaal wordt als eerste verzonden
            z+=2;
         }
         *(B+x)=b;
      }
      // xor all bytes to check the checksum value?
      DataBlock.SourceUnit=DataBlock.SourceUnit &0x1f;
      //==================================================================================
// Er zijn een aantal type Nodo events die op verschillende wijze worden behandeld:
//#define NODO_TYPE_EVENT                                  1
//#define NODO_TYPE_PLUGIN_EVENT                           4
//#define NODO_TYPE_PLUGIN_COMMAND                         5
      if ((DataBlock.Command == 4) && (DataBlock.Type == 1)){  // event->command = DataBlock.Command = 4 = EVENT_VARIABLE
         // ----------------------------------
         // Output
         // ----------------------------------
         sprintf(pbuffer, "20;%02X;", PKSequenceNumber++);    // Node and packet number 
         Serial.print( pbuffer );
         // ----------------------------------
         Serial.print(F("Slave;"));                          // Label
         sprintf(pbuffer, "ID=%02x%02x;", DataBlock.Par1, DataBlock.SourceUnit); // ID    
         Serial.print( pbuffer );
         //==================================================================================
         if ((DataBlock.Par1 < 5)  || (DataBlock.Par1 > 16)) {  // Unsupported (for now) variable, just show the contents
            int varvalue = ul2float(DataBlock.Par2);         // convert
            sprintf(pbuffer, "DEBUG=%04x;", varvalue);       // value
         } else
         if ( DataBlock.Par1 < 14) {                         // Variable 11 12 or 13 : emulate temperature sensor
            int temperature = 10 * ul2float(DataBlock.Par2); // convert
            if (temperature <= 0) temperature=-temperature | 0x8000; // set high bit for negative temperatures
            sprintf(pbuffer, "TEMP=%04x;", temperature);     // value
         } else 
         if (DataBlock.Par1 == 14){                          // Variable 14 : emulate humidity sensor
            int humidity = ul2float(DataBlock.Par2) + 0.5;   // add 0.5 to make sure it's rounded the way it should and assign as integer to remove decimal value 
            sprintf(pbuffer, "HUM=%02d;", humidity);         // value
         } else
         if (DataBlock.Par1 == 15){                          // Variable 15 : UV sensor
            int light = ul2float(DataBlock.Par2);            // supplied is value between 0 and 1024
            light = map(light, 0,1024,1,100);                // Map value to 1 - 100
            sprintf(pbuffer, "UV=%04x;", light);             // value
         } else
         if (DataBlock.Par1 == 16){                          // Variable 16 : Barometric pressure sensor
            int baro = ul2float(DataBlock.Par2);             // convert    
            sprintf(pbuffer, "BARO=%04x;", baro);            // value
         } else
         if (DataBlock.Par1 == 5){                           // Variable 5 : temperature
            int temperature = 10 * ul2float(DataBlock.Par2); // convert
            if (temperature <= 0) temperature=-temperature | 0x8000; // set high bit for negative temperatures
            sprintf(pbuffer, "TEMP=%04x;", temperature);     // value
         } else 
         if (DataBlock.Par1 == 6){                           // Variable 6 : humidity 
            int humidity = ul2float(DataBlock.Par2) + 0.5;   // add 0.5 to make sure it's rounded the way it should and assign as integer to remove decimal value 
            sprintf(pbuffer, "HUM=%02d;", humidity);         // value
         } else
         if (DataBlock.Par1 == 7){                           // Variable 7 : Rain in mm.
            int rain = ul2float(DataBlock.Par2);             // convert    
            sprintf(pbuffer, "RAIN=%04x;", rain);            // value
         } else
         if (DataBlock.Par1 == 8){                           // Variable 8 : Wind speed
            int winsp = ul2float(DataBlock.Par2);            // convert    
            sprintf(pbuffer, "WINSP=%04x;", winsp);          // value
         } else
         if (DataBlock.Par1 == 9){                           // Variable 9 : Wind Direction
            int windir = ul2float(DataBlock.Par2);           // convert    
            sprintf(pbuffer, "WINDIR=%04d;", windir);        // value
         } else
         if (DataBlock.Par1 == 10){                          // Variable 10: Wind Gust
            int wings = ul2float(DataBlock.Par2);            // convert    
            sprintf(pbuffer, "WINGS=%04x;", wings);          // value
         }  
         Serial.print( pbuffer );
         Serial.println();
         //==================================================================================
      } else {
         if ((DataBlock.Command == 0) && (DataBlock.Type==0)) return false;
         // ----------------------------------
         // Output
         // ----------------------------------
         sprintf(pbuffer, "20;%02X;", PKSequenceNumber++);    // Node and packet number 
         Serial.print( pbuffer );
         // ----------------------------------
         Serial.print(F("Slave;DEBUG="));                    // Label
         sprintf(pbuffer, "%02x %02x %02x %02x ", DataBlock.Command, DataBlock.Type, DataBlock.SourceUnit, DataBlock.Par1); 
         Serial.print( pbuffer );
         sprintf(pbuffer, "%d;", DataBlock.Par2);             // ID    
         Serial.print( pbuffer );
         Serial.println();
      }
      //==================================================================================
      RawSignal.Repeats=true;                    // suppress repeats of the same RF packet         
      RawSignal.Number=0;
      return true;
}

#endif // PLUGIN_090
