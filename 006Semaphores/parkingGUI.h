#ifndef PARKING_GUI_H
#define PARKING_GUI_H

#include <stdbool.h>

bool gui_init(void);
void gui_set_status(int available, int total, int active);
void gui_set_slot_car(int slot, int car_id);
bool gui_process_frame(void);
void gui_close(void);
void gui_set_finished(bool finished);

#endif