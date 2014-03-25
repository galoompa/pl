#ifndef _interlock_h_
#define _interlock_h_

void interlock_setup();
void interlock_loop();

bool interlocked_left();
bool interlocked_right();
bool interlocked_up();
bool interlocked_down();

void set_override_sling_raised( bool override );

#endif
