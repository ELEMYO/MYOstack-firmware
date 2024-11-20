//  Gets sensors data from sender and write them to Serial.
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

// One message data: 13 rows with 9 sensors data (234 bytes)
short sensorsData[13][9];

// callback function that will be executed when data is received
void receivedMessage(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&sensorsData, incomingData, sizeof(sensorsData));

  // Print sensors data to Serial Port
  for (int i=0; i<13; i++)
  {
    for (int j=0; j<8; j++)
    {
      Serial.print(sensorsData[i][j]);
      Serial.print(";");
    }
    Serial.println(sensorsData[i][8]);
  }
}
 
void setup() {
  Serial.begin(1000000); // initialize Serial Monitor
  
  WiFi.mode(WIFI_STA); // set receiver as a Wi-Fi Station

  esp_now_init(); // ESP-NOW initialisation
  
  esp_now_register_recv_cb(receivedMessage); // Receives message with sensors data
}
 
void loop() {

}
