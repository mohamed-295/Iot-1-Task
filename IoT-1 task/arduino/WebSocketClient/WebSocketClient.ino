#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

WebSocketsClient wsc;

const char* ssid = "";
const char* password = "";

#define SERVER  "192.168.1.8"
#define PORT		3000
#define URL			"/"

const int ledPin = D1; 

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED: {
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
			wsc.sendTXT("Connected ,hi im nodeMCU");
		}
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);

			// send message to server
			// webSocket.sendTXT("message here");
			break;
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

	Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

	// server address, port and URL
	wsc.begin(SERVER, PORT, URL);

	// event handler
	wsc.onEvent(webSocketEvent);

	// try ever 5000 again if connection has failed
	wsc.setReconnectInterval(5000);
}

void loop() {
	wsc.loop();
}
