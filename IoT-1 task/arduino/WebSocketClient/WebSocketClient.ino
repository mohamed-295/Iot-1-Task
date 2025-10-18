#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

WebSocketsClient wsc;

const char* ssid = "";
const char* password = "";

#define SERVER  "192.168.1.7"
#define PORT		3000
#define URL			"/"

const int ledPin = D1; 

int currentMode = 0 ; 
unsigned long lastTime = 0;
const unsigned long toggleInterval = 500; 
int brightness = 0;
bool ledState = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Disconnected!");
      break;

    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to server: %s\n", payload);
      wsc.sendTXT("NodeMCU connected successfully!");
      break;


    case WStype_TEXT: {
      String msg = String((char*)payload);
      msg.trim();
      //Serial.printf("Received message: %s\n", msg.c_str());

			// send message to server when Connected
		
      if (msg == "ON") {
        analogWrite(ledPin, brightness);
        currentMode = 1;
      } 
      else if (msg == "OFF") {
        analogWrite(ledPin, 0);
        currentMode = 0;
      } 
      else if (msg == "TOGGLE") {
        currentMode = 2;
        ledState = false;
        ledState = !ledState;
        lastTime = millis();
      } 
      else {
        // If message is a number (brightness 0–255)
        brightness = msg.toInt();
        if (brightness >= 0 && brightness <= 255) {
          analogWrite(ledPin, brightness);
          Serial.printf("Brightness set to %d\n", brightness);
        }
      }
      break;
    }
  }
}


void setup() {
	// USE_SERIAL.begin(921600);
	Serial.begin(115200);
	pinMode(ledPin, OUTPUT); 

 // --- Connect to Wi-Fi ---
  WiFi.begin(ssid, password);
	Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {    // Wait until connected
    delay(500);
    Serial.print(".");
  }
	Serial.println(WiFi.SSID());
	Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

	// server address, port and URL
	wsc.begin(SERVER, PORT, URL);

	// event handler
	wsc.onEvent(webSocketEvent);

	// try ever 5000 again if connection has failed
	wsc.setReconnectInterval(3000);
}

void loop() {

	if(currentMode == 2) {
    unsigned long currentTime = millis();
    if (currentTime - lastTime >= toggleInterval) {
      ledState = !ledState;
      analogWrite(ledPin, ledState ? brightness : 0);
      lastTime = currentTime;
    }
  }
	wsc.loop();
}
