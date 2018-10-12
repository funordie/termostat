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

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <ArduinoJson.h>

static void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

// Update these with values suitable for your network.
static const char* mqtt_server = "192.168.1.77";
static const uint16_t mqtt_port = 1883;

static int temperature_setpoint;
static int temperature_external;

#define JSON_TEMPERATURE_INDEX 13
#define JSON_TEMPERATURE_INDEX_SP 12

WiFiClient espClient;
PubSubClient client(espClient);

#if 0
StaticJsonBuffer<300> JSONbuffer;
#else
DynamicJsonBuffer JSONbuffer;
#endif

static void mqtt_subscribe() {
//    client.subscribe("domoticz/in");
    client.subscribe("domoticz/out");
	Serial.print(__FUNCTION__);
	Serial.print(__LINE__);
	Serial.print("subscribe to: ");
	Serial.println("domoticz/out");
}

void mqtt_setup() {

	delay(5);
	client.setServer(mqtt_server, mqtt_port);
	client.setCallback(callback);

	if (!client.connected()) {
		reconnect();
	}

	mqtt_subscribe();
}

void callback(char* topic, byte* payload, unsigned int length) {
	Serial.print(__FUNCTION__);
	Serial.print(__LINE__);
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (unsigned int i=0;i<length;i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();

	//desearize JSON buffer
	JsonObject & obj = JSONbuffer.parseObject(payload);
	if(!obj.success())
	{
		Serial.print(__FUNCTION__);
		Serial.print(__LINE__);
		Serial.println("parse error !!!!!!");
		return;
	}
	int idx = obj["idx"];
	int nvalue = obj["nvalue"];
	int svalue = obj["svalue"];

	//TODO: process JSON data
	switch(idx) {
	case JSON_TEMPERATURE_INDEX_SP: {
		Serial.print(__FUNCTION__);
		Serial.print(__LINE__);
		Serial.print("receive Temperature Set point: ");
		Serial.print(nvalue);
		Serial.print(svalue);
		Serial.println("");
		temperature_setpoint = svalue;
		break;
	}
	case JSON_TEMPERATURE_INDEX: {
		Serial.print(__FUNCTION__);
		Serial.print(__LINE__);
		Serial.print("receive Temperature: ");
		Serial.print(nvalue);
		Serial.print(svalue);
		Serial.println("");
		temperature_external = svalue;
		break;
	}
	default:
		Serial.print(__FUNCTION__);
		Serial.print(__LINE__);
		Serial.print("unknown index:");
		Serial.print(idx);
		return;
	};
}

static void reconnect() {
	  // Loop until we're reconnected
	  while (!client.connected()) {
	    Serial.print("Attempting MQTT connection...");
	    // Attempt to connect
	    if (client.connect("arduinoClient")) {
	      Serial.println("connected");
	      // Once connected, publish an announcement...
	      client.publish("outTopic","hello world");
	      // ... and resubscribe
	      client.subscribe("inTopic");
	    } else {
	      Serial.print("failed, rc=");
	      Serial.print(client.state());
	      Serial.println(" try again in 5 seconds");
	      // Wait 5 seconds before retrying
	      delay(5000);
	    }
	  }
}
void mqtt_loop() {

	if (!client.connected()) {
		reconnect();
	}
	client.loop();
}

int mqtt_publish_temperature(float temperature) {

	JsonObject & obj = JSONbuffer.createObject();
	obj["idx"] = JSON_TEMPERATURE_INDEX;
	obj["svalue"] = String(temperature);
	obj["nvalue"] = 0;
	//JSONencoder["HUM"] = h;
	char JSONmessageBuffer[100];
	obj.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

	Serial.println("publish temperature debug start");
	Serial.print(__FUNCTION__);
	Serial.print(__LINE__);
	Serial.print(JSONmessageBuffer);
	Serial.println("publish temperature debug end");
	client.publish("domoticz/in",JSONmessageBuffer,false);

	//TODO: check for error
	return 0;
}

int mqtt_get_setpoint(float * value) {

	*value = temperature_setpoint;

	//TODO: check for error
	return 0;
}

int mqtt_get_temperature(float * value) {

	*value = temperature_external;

	//TODO: check for error
	return 0;
}
