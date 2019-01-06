/*
 * wifi.cpp
 *
 *  Created on: Oct 12, 2018
 *      Author: ipaev
 */
#include <WiFiManager.h>

#include <common.hpp>

static int wifi_connect() {
    if (WiFi.status() != WL_CONNECTED) {
        addToLog(LOG_LEVEL_ERROR, "WIFI is not connected");
        WiFiManager wifiManager;
        if(!wifiManager.autoConnect()) {
            //WIFI is not connected
            addToLog(LOG_LEVEL_ERROR, "WIFI reconnect failed !!!!!");
            return -1;
        }
        else {
            addToLog(LOG_LEVEL_ERROR, "WIFI reconnect OK");
        }
    }
    return 0;
}

void wifi_setup() {

    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);
	pinMode(TRIGGER_PIN, INPUT);

	if ( digitalRead(TRIGGER_PIN) == LOW ) {

	    //WiFiManager
	    //Local intialization. Once its business is done, there is no need to keep it around
	    WiFiManager wifiManager;

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
		if(wifi_connect()) {
		    addToLog(LOG_LEVEL_ERROR, "failed to connect and hit timeout");
		}
	}
	addToLog(LOG_LEVEL_ERROR, "WiFi connected: %s %s", WiFi.localIP().toString().c_str(), WiFi.SSID().c_str());
}

int wifi_check() {
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);

    return wifi_connect();
}
