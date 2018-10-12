/*
 * oled.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

#include <gpio_define.hpp>
static SSD1306Wire  display(0x3c, SSD1306_SDA, SSD1306_SCL);

void oled_setup() {
	// Initialising the UI will init the display too.
	display.init();

	display.flipScreenVertically();
	display.setFont(ArialMT_Plain_10);
}

void oled_loop() {
}

int oled_clear() {
    display.clear();
    //TODO: check for errors
    return 0;
}
int oled_print(int16_t xMove, int16_t yMove, String strUser) {
    display.drawString(xMove, yMove, strUser);
    //TODO: check for errors
    return 0;
}

int oled_display() {
    display.display();
    //TODO: check for errors
    return 0;
}
