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
	printf("%s:%d\n",__FUNCTION__, __LINE__);
	sensors.begin();
}

void temperature_loop() {
	printf("%s:%d\n",__FUNCTION__, __LINE__);
	// call sensors.requestTemperatures() to issue a global temperature
	// request to all devices on the bus
	printf("Requesting temperatures...\n");
	sensors.requestTemperatures(); // Send the command to get temperatures
	printf("DONE\n");
	// After we got the temperatures, we can print them here.
	// We use the function ByIndex, and as an example get the temperature from the first sensor only.

	temperature = sensors.getTempCByIndex(0);
	printf("Temperature for the device 1 (index 0) is: %f\n", temperature);
}

int temperature_get_temp(float * fTemp) {

	if(fTemp == NULL) {
		printf("%s null pointer\n", __FUNCTION__);
		return -1;
	}
	*fTemp = temperature;
	return 0;
}
