//  Collects sensors data and sends it to receiver board using ESP-NOW protocol.
//  2024-11-19 by ELEMYO (https://github.com/ELEMYO/Elemyo-library)
//
//  Changelog:
//  2024-11-19 - simple data compression added
//  2021-05-02 - initial release
//  

/* ============================================
ELEMYO library code is placed under the MIT license

Copyright (c) 2021 ELEMYO
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
==============================================
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

// Read sensor data by I2C interface
void sensorDataRead (byte* dta, byte Addr);

// Replace with receiver MAC address
uint8_t receiverMACAddress[] = {0x34, 0x85, 0x18, 0x04, 0xC9, 0x58};

// I2C addresses of sensors
uint8_t sensorAddr [] = {0x50, 0x51, 0x52, 0x54, 0x55, 0x56, 0x58, 0x59, 0x5A};

// One message data: 27 rows with 9 sensors data (243 bytes)
byte sensorsData[27][9];

esp_now_peer_info_t peer; // peer information

void setup() {
  Wire.begin(); // initiate the Wire library and join the I2C bus as a master
  Wire.setClock(530000); // set clock frequency for I2C communication
  delay(300);
 
  WiFi.mode(WIFI_STA); // set sender as a Wi-Fi Station
  WiFi.setTxPower(WIFI_POWER_MINUS_1dBm); // set Wi-Fi signal power

  esp_now_init(); // ESP-NOW initialisation
  
  memcpy(peer.peer_addr, receiverMACAddress, 6);
  peer.channel = 0;  
  peer.encrypt = false;
          
  esp_now_add_peer(&peer); // add peer
}
 
void loop() {
  while(1){
  byte data[2];
  
  // Updating message data: 27 rows with 9 sensors data
  for (int i=0; i<27; i=i+3)
  {
      for (int j=0; j<9; j++)
      {
        sensorDataRead(data, sensorAddr[j]);  
        sensorsData[i][j] = data[0];
        sensorsData[i+1][j] = data[1];
      }
      
      for (int j=0; j<9; j++)
      {
        sensorDataRead(data, sensorAddr[j]);
        sensorsData[i][j] = (data[0] << 4) | (sensorsData[i][j]);  
        sensorsData[i+2][j] = data[1];
      }
  }
  
  // Send message with sensors data to receiver
  esp_err_t result = esp_now_send(receiverMACAddress, (byte *) &sensorsData, sizeof(sensorsData));
  }
}

void sensorDataRead (byte* data, byte Addr)
{
  uint8_t bytesReceived = 0;

  bytesReceived = Wire.requestFrom(Addr, 2, true);
  
  if (bytesReceived == 2)
    Wire.readBytes(data, bytesReceived);
  else
  {
    short ref = 2048;
    data[0] = highByte(ref);
    data[1] = lowByte(ref);
    delayMicroseconds(62);
  }
}
