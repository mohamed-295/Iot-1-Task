#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

WebSocketsClient wsc;

const char* ssid = "";
const char* password = "";

#define SERVER  "192.168.1.5"
#define PORT		3000
#define URL			"/"

const int ledPin = D1; 

int currentBrightness = 0; // current LED PWM
int targetBrightness = 0;  // target PWM from server

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Disconnected!");
      break;

    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to server: %s\n", payload);
      wsc.sendTXT("NodeMCU connected successfully!");
      break;

<<<<<<< HEAD
    case WStype_TEXT: {
      String msg = String((char*)payload);
      msg.trim();
      Serial.printf("Received message: %s\n", msg.c_str());
=======
			// send message to server when Connected
			wsc.sendTXT("Connected ,hi im nodeMCU");
		}
			break;
		case WStype_TEXT:
		 {
				USE_SERIAL.printf("[WSc] get text: %s\n", payload);

        String msg = String((char*)data);

        msg.replace("[", "");
        msg.replace("]", "");
        msg.replace("'", "");
        msg.replace(",", "");
        msg.trim();

        int brightness = msg.toInt();
        brightness = constrain(brightness, 0, 255);
        targetBrightness = map(brightness, 0, 255, 0, 1023);

        Serial.printf("Target brightness set to: %d\n", targetBrightness);
      }
			
>>>>>>> 4c2bf03f12306900137c6f54cb4a85a25a0e80c8

      if (msg == "ON") {
        analogWrite(ledPin, 255);
      } 
      else if (msg == "OFF") {
        analogWrite(ledPin, 0);
      } 
      else if (msg == "TOGGLE") {
        static bool ledState = false;
        ledState = !ledState;
        analogWrite(ledPin, ledState ? 255 : 0);
      } 
      else {
        // If message is a number (brightness 0–255)
        int brightness = msg.toInt();
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
	USE_SERIAL.begin(115200);
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
	if (currentBrightness < targetBrightness) currentBrightness++;
  else if (currentBrightness > targetBrightness) currentBrightness--;

  analogWrite(LED_PIN, currentBrightness);
	
	wsc.loop();
}
