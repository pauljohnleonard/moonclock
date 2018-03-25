#ifndef _SERVO_H
#define _SERVO_H



void servo_attach();

void servo_detach();


bool servo_attached();

void servo_writeMs(int mS);

int servo_readV();

#endif
