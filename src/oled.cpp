/*
 * oled.cpp
 *
 *  Created on: Oct 7, 2018
 *      Author: ipaev
 */

#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include "OLEDDisplayUi.h"
#include <common.hpp>
#include "oled.hpp"
#include "rtc.hpp"

#include "oled_fonts.hpp"

const uint8_t activeSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B01000010,
    B01000010,
    B00100100,
    B00011000
};

const uint8_t inactiveSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000
};

////Added by Sloeber
//#pragma once

SSD1306Wire display(0x3c, SSD1306_SDA, SSD1306_SCL);
OLEDDisplayUi ui     ( &display );

oled_display_status_t   dspl_status;
String                  dspl_loading;
////////////////   OVERLAY   ///////////////////////////////////

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(10, 0, GetDateAndTime());
}

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

//////////////////   FRAMES    ////////////////////////////////////

void drawFrameLoading(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//    addToLog(LOG_LEVEL_ERROR, "%s: enter", __FUNCTION__);

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(Dialog_plain_12);
    display->drawString(20, 20, dspl_loading);
}

void drawFrameStatus(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//    addToLog(LOG_LEVEL_ERROR, "%s: enter", __FUNCTION__);

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(Dialog_plain_12);

#define SIZE 16
    //OVERLAY ROW
    display->drawString(0, SIZE, String("In:") + String(dspl_status.temp, 1));
    display->drawString(50, SIZE, String("Out:") + String(dspl_status.temp_out, 1));
    display->drawString(0, SIZE * 2, String("SP:") + String(dspl_status.temp_sp, 1));
    display->drawString(50,SIZE * 2, String("Mode:") + String(dspl_status.mode));
    display->drawString(0, SIZE * 3, String("Rel: ") + String(dspl_status.relay));
    display->drawString(40, SIZE * 3, dspl_status.info);
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrameLoading, drawFrameStatus};

// how many frames are there?
int frameCount = 2;

//////////////////////////////////////////////////////////////////////////////


void oled_setup() {
    addToLog(LOG_LEVEL_DEBUG_MORE, "%s: enter", __FUNCTION__);

    display.init();

    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

    // The ESP is capable of rendering 60fps in 80Mhz mode
    // but that won't give you much time for anything else
    // run it in 160Mhz mode or just set it to 30 fps
    ui.setTargetFPS(60);

    // Customize the active and inactive symbol
    ui.setActiveSymbol(activeSymbol);
    ui.setInactiveSymbol(inactiveSymbol);

    // You can change this to
    // TOP, LEFT, BOTTOM, RIGHT
    ui.setIndicatorPosition(RIGHT);

    // Add overlays
    ui.setOverlays(overlays, overlaysCount);

    // Add frames
    ui.setFrames(frames, frameCount);

    ui.disableAutoTransition();
}

void oled_loop() {
//    addToLog(LOG_LEVEL_ERROR, "%s: enter", __FUNCTION__);
    int remainingTimeBudget = ui.update();
}

void oled_switchToFrame(uint8_t frame) {
    ui.switchToFrame(frame);
}
