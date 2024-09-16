/*++++++++++++
  thanet Y.
  16/9/2024 
  for test espnow
  -------------*/

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <Arduino_JSON.h>
#include <HTTPClient.h> 


// Public variables
int readmoduleno = 0;
float temperature = 0; 
float humidity = 0;
int readId = 0;
bool readytoupload = false;

// Prototype function
void initializeEspNow();

// Structure to receive data
typedef struct struct_message {
  int read_module_no;
  float temp;
  float hum;
  unsigned int readingId;
} struct_message;

struct_message incomingReadings;

JSONVar board;

// Callback function for received data
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  
  board["read_module_no"] = incomingReadings.read_module_no;
  board["temperature"] = incomingReadings.temp;
  board["humidity"] = incomingReadings.hum;
  board["readingId"] = incomingReadings.readingId;
  String jsonString = JSON.stringify(board);
  //events.send(jsonString.c_str(), "new_readings", millis());
  
  Serial.printf("Board ID %u: %u bytes\n", incomingReadings.read_module_no, len);
  Serial.printf("Temperature: %4.2f \n", incomingReadings.temp);
  Serial.printf("Humidity: %4.2f \n", incomingReadings.hum);
  Serial.printf("Reading ID: %d \n", incomingReadings.readingId);
  Serial.println();

  readmoduleno = incomingReadings.read_module_no;
  temperature = incomingReadings.temp;
  humidity = incomingReadings.hum;
  readId = incomingReadings.readingId;
  readytoupload = true;
}

void setup(){
  Serial.begin(115200);

   // Initialize WiFi and select network configuration
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(WIFI_PS_NONE);

  // for initialze EspNow
  initializeEspNow();
}
 
void loop(){
  Serial.println("waiting for data to come..");
  Serial.println("..");
  delay(5000);

}

void initializeEspNow() {
// Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register for received data callback
  esp_now_register_recv_cb(OnDataRecv);

}