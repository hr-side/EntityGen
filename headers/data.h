#ifndef __DATA_H
#define __DATA_H
#include <ui.h>
#include <canvas.h>

void Export_Data(Canvas *c, Ui_State *ui);
void Import_Data(Canvas *c, Ui_State *ui);
bool Parse(Ui_State *ui, Canvas *c);

#endif //__DATA_H
