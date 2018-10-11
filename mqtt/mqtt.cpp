/*
 * mqtt.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

 */
#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <ArduinoJson.h>

#include <gpio_define.hpp>

void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);

// Update these with values suitable for your network.
const char* mqtt_server = "192.168.1.77";

static int mqtt_setpoint;

#define JSON_TEMPERATURE_INDEX 1
#define JSON_TEMPERATURE_INDEX_SP 99

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

StaticJsonBuffer<300> JSONbuffer;

void mqtt_setup() {
	printf("%s:%d\n",__FUNCTION__, __LINE__);
	setup_wifi();

	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);

	pinMode(TRIGGER_PIN, INPUT);
}

void setup_wifi() {

	//WiFiManager
	//Local intialization. Once its business is done, there is no need to keep it around
	WiFiManager wifiManager;

	if ( digitalRead(TRIGGER_PIN) == LOW ) {

		//reset settings - for testing
		//wifiManager.resetSettings();

		//sets timeout until configuration portal gets turned off
		//useful to make it all retry or go to sleep
		//in seconds
		wifiManager.setTimeout(5*60); //5 minute timeout

		//WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
		//WiFi.mode(WIFI_STA);

		if (!wifiManager.startConfigPortal("IP_AP", "IP_AP_PASS")) {
			Serial.println("failed to connect and hit timeout");
			delay(3000);
			//reset and try again, or maybe put it to deep sleep
			ESP.reset();
			delay(5000);
		}

		//if you get here you have connected to the WiFi
		Serial.println("connected...yeey :)");
	}
	else {
		wifiManager.setTimeout(120);
		int count = 0;
		while(!wifiManager.autoConnect() && count < 3) {
			printf("failed [%d] to connect and hit timeout\n", count);
			delay(5000);
			count++;
		};
	}
	printf("WiFi connected\n  IP address: %s\n", WiFi.localIP().toString().c_str());
}

void callback(char* topic, byte* payload, unsigned int length) {
	printf("Message arrived [%s]", topic);

	printf("Message payload:[");
	for (unsigned int i = 0; i < length; i++) {
		printf("%c", payload[i]);
	}
	printf("]\n");

	//desearize JSON buffer
	JsonObject & obj = JSONbuffer.parseObject(payload);
	if(!obj.success())
	{
		printf("%s:%d: parse error !!!!!!\n", __FUNCTION__, __LINE__);
		return;
	}
	int idx = obj["idx"];
	int nvalue = obj["nvalue"];
	int svalue = obj["svalue"];

	//TODO: process JSON data
	switch(idx) {
	case JSON_TEMPERATURE_INDEX_SP: {
		printf("%s:%d: receive Temperature Set point: %d -> %d\n", __FUNCTION__, __LINE__, svalue , nvalue);
		break;
	}
	default:
		printf("%s:%d: Unknown index\n", __FUNCTION__, __LINE__);
		return;
	};
}

static void reconnect() {
	// Loop until we're reconnected
	while (!client.connected()) {
		printf("Attempting MQTT connection...\n");
		// Attempt to connect
		if (client.connect("ESP8266Client")) {
			printf("connected\n");
			// Once connected, publish an announcement...
			client.publish("outTopic", "hello world");
			// ... and resubscribe
			client.subscribe("inTopic");
		} else {
			printf("failed, rc=%d try again in 5 seconds\n", client.state());
			// Wait 5 seconds before retrying
			delay(5000);
		}
	}
}
void mqtt_loop() {

	printf("%s:%d\n",__FUNCTION__, __LINE__);
	if (!client.connected()) {
		reconnect();
	}
	client.loop();

	long now = millis();
	if (now - lastMsg > 2000) {
		lastMsg = now;
		//TODO: alive message
	}
}

int mqtt_publish_temperature(float temperature) {

	JsonObject & obj = JSONbuffer.createObject();
	obj["idx"] = JSON_TEMPERATURE_INDEX;
	obj["nvalue"] = temperature;
	obj["svalue"] = 0;
	//JSONencoder["HUM"] = h;
	char JSONmessageBuffer[100];
	obj.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
	client.publish("domoticz/in",JSONmessageBuffer,false);

	//TODO: check for error
	return 0;
}

int mqtt_get_setpoint(float * setpoint) {

	*setpoint = mqtt_setpoint;

	//TODO: check for error
	return 0;
}
