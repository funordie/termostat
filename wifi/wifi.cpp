/*
 * wifi.cpp
 *
 *  Created on: Oct 12, 2018
 *      Author: ipaev
 */
#include <WiFiManager.h>

#include <common.hpp>

void wifi_setup() {

    addToLog(LOG_LEVEL_ERROR,"%s:%d", __FUNCTION__, __LINE__);
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
		    addToLog(LOG_LEVEL_ERROR, "failed to connect and hit timeout");
			delay(3000);
			//reset and try again, or maybe put it to deep sleep
			ESP.reset();
			delay(5000);
		}

		//if you get here you have connected to the WiFi
		addToLog(LOG_LEVEL_ERROR, "connected...yeey :)");
	}
	else {
		wifiManager.setTimeout(120);
		int count = 0;
		while(!wifiManager.autoConnect() && count < 3) {
		    addToLog(LOG_LEVEL_ERROR, "failed to connect and hit timeout");
			delay(5000);
			count++;
		};
	}
	addToLog(LOG_LEVEL_ERROR, "WiFi connected: %s %s", WiFi.localIP().toString().c_str(), WiFi.SSID().c_str());
}

