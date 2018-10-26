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

    addToLog(LOG_LEVEL_ERROR, "subscribe to: sp:%s mode:%s\n", topic_sp.c_str(), topic_mode.c_str());
}

void mqtt_setup() {

    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter\n", __FUNCTION__);
	while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
		delay(250);
		addToLog(LOG_LEVEL_ERROR, '.');
	}

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
    addToLog(LOG_LEVEL_ERROR, "Message arrived [%s] ", topic);
	for (unsigned int i=0;i<length;i++) {
	    addToLog(LOG_LEVEL_ERROR, "%c", (char)payload[i]);
	}
	addToLog(LOG_LEVEL_ERROR, "\n");

	if(!strcmp(topic, topic_sp.c_str())) {
	    float value = String((char*)payload).toFloat();  //atof((char*)payload);
	    addToLog(LOG_LEVEL_ERROR, "receive Temperature Set point float: %f\n", value);
		temperature_setpoint = value;
	}
    if(!strcmp(topic, topic_mode.c_str())) {
	    int value = String((char*)payload).toInt();
	    addToLog(LOG_LEVEL_ERROR, "receive Temperature: %d\n", value);
		temperature_mode = value;
	}
}

static void reconnect() {
	  // Loop until we're reconnected
	  while (!client.connected()) {
	      addToLog(LOG_LEVEL_ERROR, "Attempting MQTT connection...\n");
	    // Attempt to connect
	    if (client.connect("arduinoClient")) {
	      addToLog(LOG_LEVEL_ERROR, "connected\n");
	      // Once connected, publish an announcement...
	      client.publish("outTopic","hello world");
	      // ... and resubscribe
	      client.subscribe("inTopic");
	    } else {
	      addToLog(LOG_LEVEL_ERROR, "failed, rc=%d try again in 5 seconds\n", client.state());
	      // Wait 5 seconds before retrying
	      delay(5000);
	    }
	  }
}
void mqtt_loop() {

    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter\n", __FUNCTION__);
	if (!client.connected()) {
		reconnect();
		mqtt_subscribe();
	}
	client.loop();
}

int mqtt_publish_temperature(float temperature) {

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
