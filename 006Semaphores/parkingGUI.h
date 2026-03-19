#ifndef PARKING_GUI_H
#define PARKING_GUI_H

#include <stdbool.h>

bool gui_init(void);
void gui_set_spaces(int available, int total);
bool gui_process_frame(void);
void gui_close(void);

#endif