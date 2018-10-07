/*
 * oled.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
SSD1306Wire  display(0x3c, D5, D1);

void oled_setup() {
    // Initialising the UI will init the display too.
    display.init();

    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
}

void oled_loop() {
    display.setFont(ArialMT_Plain_10);

    // The coordinates define the left starting point of the text
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 10, "Left aligned (0,10)");

    // write the buffer to the display
    display.display();
}
