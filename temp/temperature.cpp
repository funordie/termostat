/*
 * temperature.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

#include <OneWire.h>
#include <DallasTemperature.h>

#include <gpio_define.hpp>

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(DS18b20_PIN);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

static float temperature;

void tempetarure_setup() {
	sensors.begin();
}

void temperature_loop() {
	// call sensors.requestTemperatures() to issue a global temperature
	// request to all devices on the bus
	sensors.requestTemperatures(); // Send the command to get temperatures
	// After we got the temperatures, we can print them here.
	// We use the function ByIndex, and as an example get the temperature from the first sensor only.
	temperature = sensors.getTempCByIndex(0);
}

int temperature_get_temperature(float * fTemp) {

	if(fTemp == NULL) {
		Serial.print(__FUNCTION__);
		Serial.print(__LINE__);
		Serial.println("null pointer !!!");
		return -1;
	}
	*fTemp = temperature;
	return 0;
}
