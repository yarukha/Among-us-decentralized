#ifndef ENTERIDCODE_H
#define ENTERIDCODE_H

#include <QLabel>
#include <QMouseEvent>



#define DIGIT_NUMBER 6
#define MAX_DIGIT 9
#define DIGIT_UNDEFINED MAX_DIGIT +1

QLabel* getEnterIDCode();

void onCloseEnterIDCode(), resetCode();

#endif