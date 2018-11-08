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

    addToLog(LOG_LEVEL_ERROR, "subscribe to: sp:%s mode:%s", topic_sp.c_str(), topic_mode.c_str());
}

void mqtt_setup() {

    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);
	if (WiFi.status() != WL_CONNECTED) {
		addToLog(LOG_LEVEL_ERROR, "%s:WiFi.status() != WL_CONNECTED ", __FUNCTION__);
	}

    topic_sp = String(ESP.getChipId()) + "/topic" + "/setpoint";
    topic_mode = String(ESP.getChipId()) + "/topic" + "/mode";
    topic_temp = String(ESP.getChipId()) + "/topic" + "/temperature";

	delay(5000);
	client.setServer(mqtt_server, mqtt_port);
	client.setCallback(callback);

	if (!client.connected()) {
        if (client.connect("arduinoClient"))
          addToLog(LOG_LEVEL_INFO, "mqtt: connected");
	}
    else {
        addToLog(LOG_LEVEL_INFO, "mqtt: already connected");
    }

	mqtt_subscribe();
}

void callback(char* topic, byte* payload, unsigned int length) {
    addToLog(LOG_LEVEL_DEBUG, "Message arrived [%s] ", topic);

    //payload array, do not have terminating character.
    //Allocate new string with size "length + 1"
    char * buffer = NULL;
    buffer = new char[length + 1];
    if(!buffer) {
        addToLog(LOG_LEVEL_ERROR, "mqtt: cannot allocate payload buffer");
        return;
    }
    strncpy(buffer, (char*)payload, length);
    buffer[length + 1] = 0;

 	for (unsigned int i=0;i<length;i++) {
	    addToLogEx(LOG_LEVEL_DEBUG, "%c", (char)buffer[i]);
	}
	addToLog(LOG_LEVEL_DEBUG, "");

	if(!strcmp(topic, topic_sp.c_str())) {
	    float value = String(buffer).toFloat();
	    addToLog(LOG_LEVEL_DEBUG, "receive Temperature Set point float: %f", value);
		temperature_setpoint = value;
	}
    if(!strcmp(topic, topic_mode.c_str())) {
	    int value = String(buffer).toInt();
	    addToLog(LOG_LEVEL_DEBUG, "receive Temperature: %d", value);
		temperature_mode = value;
	}

    if(buffer) free(buffer);
}

void mqtt_loop() {

    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);
	client.loop();
}

int mqtt_check() {
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);
    if (!client.connected()) {
        addToLog(LOG_LEVEL_ERROR, "MQTT is not connected");
        String ssid = "ESP" + String(ESP.getChipId());
        if (!client.connect(ssid.c_str())) {
            //mqtt is not connected
            addToLog(LOG_LEVEL_ERROR, "MQTT reconnect failed !!!!!");
            return -1;
        }
        //mqtt is connected
        addToLog(LOG_LEVEL_ERROR, "MQTT reconnect OK");
        mqtt_subscribe();
    }
    return 0;
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
