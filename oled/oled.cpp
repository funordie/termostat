/*
 * oled.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

#include <gpio_define.hpp>
SSD1306Wire  display(0x3c, SSD1306_SDA, SSD1306_SCL);

void oled_setup() {
	printf("%s:%d\n",__FUNCTION__, __LINE__);
	// Initialising the UI will init the display too.
	display.init();

	display.flipScreenVertically();
	display.setFont(ArialMT_Plain_10);
}

void oled_loop() {
	printf("%s:%d\n",__FUNCTION__, __LINE__);
	display.setFont(ArialMT_Plain_10);

	// The coordinates define the left starting point of the text
	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.drawString(0, 10, "Left aligned (0,10)");

	// write the buffer to the display
	display.display();
}
