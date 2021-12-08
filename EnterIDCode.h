#ifndef ENTERIDCODE_H
#define ENTERIDCODE_H

#include <QLabel>
#include <QMouseEvent>



#define DIGIT_NUMBER 6
#define MAX_DIGIT 9
#define DIGIT_UNDEFINED MAX_DIGIT +1

#define X_TOP_LEFT_1 277
#define Y_TOP_LEFT_1 67
#define X_LENGTH 66
#define Y_LENGTH 66
#define X_SPACE 18
#define Y_SPACE 12



QPair<QPixmap*, QPainter*> getEnterIDCodePixmapPainter();
QLabel* getEnterIDCode();

void onMouseEventEnterIDCode(QMouseEvent* mouseEvent),
    onCloseEnterIDCode(),
    resetCode();

#endif