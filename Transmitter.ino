//  Collects sensors data and sends it to receiver board using ESP-NOW protocol.
//  2021-05-02 by ELEMYO (https://github.com/ELEMYO/Elemyo-library)
//
//  Changelog:
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
int sensorDataRead (byte Addr);

// Replace with receiver MAC address
uint8_t receiverMACAddress[] = {0x7C, 0x9E, 0xBD, 0xF1, 0x4D, 0xB8};

// I2C addresses of sensors
uint8_t sensorAddr [] = {0x50, 0x51, 0x52, 0x54, 0x55, 0x56, 0x58, 0x59, 0x5A};

// One message data: 13 rows with 9 sensors data (234 bytes)
short sensorsData[13][9] = {{0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0, 0}};

void setup() {
  Wire.begin(); // initiate the Wire library and join the I2C bus as a master
  Wire.setClock(400000); // set clock frequency for I2C communication
  delay(300);
 
  WiFi.mode(WIFI_STA); // set sender as a Wi-Fi Station
  WiFi.setTxPower(WIFI_POWER_MINUS_1dBm); // set Wi-Fi signal power

  esp_now_init(); // ESP-NOW initialisation
  
  esp_now_peer_info_t peer; // peer information
  memcpy(peer.peer_addr, receiverMACAddress, 6);
  peer.channel = 0;  
  peer.encrypt = false;
          
  esp_now_add_peer(&peer); // add peer
}
 
void loop() {
  // Updating message data: 13 rows with 9 sensors data
  for (int i=0; i<13; i++)
  {
    for (int j=0; j<9; j++)
      sensorsData[i][j] = sensorDataRead(sensorAddr[j]);    
    delayMicroseconds(1125); // delay between data read
  }
  
  // Send message with sensors data to receiver
  esp_err_t result = esp_now_send(receiverMACAddress, (uint8_t *) &sensorsData, sizeof(sensorsData));
}

int sensorDataRead (byte Addr)
{
  unsigned int data[2];
  
  Wire.requestFrom(Addr, 2);
  
  if(Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
  return((data[0] & 0x0F) * 256) + data[1];
}
