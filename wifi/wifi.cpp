/*
 * wifi.cpp
 *
 *  Created on: Oct 12, 2018
 *      Author: ipaev
 */
#include <WiFiManager.h>

#include <common.hpp>

void wifi_setup() {

	pinMode(TRIGGER_PIN, INPUT);

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
			Serial.print(__FUNCTION__);
			Serial.print(__LINE__);
			Serial.println("failed to connect and hit timeout");
			delay(5000);
			count++;
		};
	}
	Serial.print(__FUNCTION__);
	Serial.print(__LINE__);
	Serial.print("WiFi connected: ");
	Serial.print(WiFi.localIP().toString().c_str());
	Serial.println(WiFi.SSID());
}

