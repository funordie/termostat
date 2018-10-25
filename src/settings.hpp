#ifndef _SETTINGS_H_
#define _SETTINGS_H_

bool TimeReached(unsigned long timer);
void SetNextTimeInterval(unsigned long& timer, const unsigned long step);

void settings_setup();
void settings_loop();

#endif  // _SETTINGS_H_
