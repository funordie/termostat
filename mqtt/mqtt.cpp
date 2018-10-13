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

#include <common.hpp>

static void reconnect();
void callback(char* topic, byte* payload, unsigned int length);

// Update these with values suitable for your network.
static const char* mqtt_server = "192.168.1.77";
static const uint16_t mqtt_port = 1883;

static float temperature_setpoint;
static float temperature_external;
static int temperature_mode;

WiFiClient espClient;
PubSubClient client(espClient);

String topic_sp;
String topic_temp;
String topic_mode;

static void mqtt_subscribe() {
    client.subscribe(topic_sp.c_str());
    client.subscribe(topic_mode.c_str());

	Serial.print(__FUNCTION__);
	Serial.print(__LINE__);
	Serial.print("subscribe to: ");
	Serial.println(topic_sp);
	Serial.print("subscribe to: ");
	Serial.println(topic_mode);
}

void mqtt_setup() {

    topic_sp = String(ESP.getChipId()) + "/topic" + "/setpoint";
    topic_mode = String(ESP.getChipId()) + "/topic" + "/mode";
    topic_temp = String(ESP.getChipId()) + "/topic" + "/temperature";

	delay(5000);
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
    char str[10];
    memcpy(str, payload, length);
    str[length] = 0;

	if(!strcmp(topic, topic_sp.c_str())) {
	    float value = String(str).toFloat();  //atof((char*)payload);
		Serial.print("receive Temperature Set point: ");
		Serial.print(value);
		Serial.println("");
		temperature_setpoint = value;
	}
    if(!strcmp(topic, topic_mode.c_str())) {
	    int value = String(str).toInt();
		Serial.print(__FUNCTION__);
		Serial.print(__LINE__);
		Serial.print("receive Temperature: ");
		Serial.print(value);
		Serial.println("");
		temperature_mode = value;
	}
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
		mqtt_subscribe();
	}
	client.loop();
}

int mqtt_publish_temperature(float temperature) {

    static long lastMsg = 0;
    if(limit_execution_time_sec(&lastMsg, 10)) return 0;

	client.publish(topic_temp.c_str(),String(temperature).c_str(),true);

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

int mqtt_get_mode(int * value) {

    *value = temperature_mode;

    //TODO: check for error
    return 0;
}
