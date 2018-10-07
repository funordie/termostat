/*
 * temperature.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS D2  	//GPIO4

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

static float temperature;

void tempetarure_setup() {
	sensors.begin();
}

void temperature_loop() {
	  // call sensors.requestTemperatures() to issue a global temperature
	  // request to all devices on the bus
	  Serial.print("Requesting temperatures...");
	  sensors.requestTemperatures(); // Send the command to get temperatures
	  Serial.println("DONE");
	  // After we got the temperatures, we can print them here.
	  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
	  Serial.print("Temperature for the device 1 (index 0) is: ");
	  temperature = sensors.getTempCByIndex(0);
	  Serial.println(temperature);
}

int temperature_get_temp(float * fTemp) {

    if(fTemp == NULL) {
        char msg[50];
        sprintf(msg, "%s null pointer", __FUNCTION__);
        Serial.println(msg);
        return -1;
    }
    *fTemp = temperature;
}
