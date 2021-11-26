#include <QLabel>
#include <QRandomGenerator>
#include <QPainter>
#include <QMediaPlayer>
#include <QFileInfo>
#include <QDateTime>
#include <QHBoxLayout>
#include <QtMath>
#include <QPainterPath>
#include <QString>
#include "EnterIDCode.h"
#include "main.h"
#include "qPlus.h"


quint8 code[DIGIT_NUMBER] = {DIGIT_UNDEFINED,DIGIT_UNDEFINED,DIGIT_UNDEFINED,DIGIT_UNDEFINED,DIGIT_UNDEFINED,DIGIT_UNDEFINED};
QPixmap* EnterIDCodeBackgroundPixmap = nullptr;
QPixmap* currEnterIDCodePixmap = nullptr;
QLabel* currEnterIDCodeLabel = nullptr;


void randomCode(){
    for (quint8 i = 0; i< DIGIT_NUMBER; i++){
        quint8 randomdigit = QRandomGenerator::global()->bounded(MAX_DIGIT);
        code[i]=randomdigit;
    }

}

QString getStringofcode(){
    QString string_code ;
    QTextStream(& string_code) << code[0] << code[1] << code[2] << code[3] <<code[4] << code[5] ;
    return string_code;
}

QPair<QPixmap*, QPainter*> getEnterIDCodePixmapPainter()
{
    QPixmap* pixmap = new QPixmap(EnterIDCodeBackgroundPixmap->size());
    QPainter* painter = new QPainter(pixmap);
    painter->drawImage(0, 0, EnterIDCodeBackgroundPixmap->toImage());
    
    QString s = getStringofcode(); 
    painter->drawStaticText(200, 300, QStaticText(s));
    return qMakePair(pixmap, painter);
}

QLabel* getEnterIDCode(){
    if(!EnterIDCodeBackgroundPixmap) {EnterIDCodeBackgroundPixmap = getQPixmap("EnterIDCode.png");};

    playSound("Fix_Wiring_task_open_sound.wav");
    QLabel* qFrame = new QLabel;
    QHBoxLayout* hbox = new QHBoxLayout(qFrame);
    QLabel* qLabel = new QLabel(qFrame);
    hbox->addStretch();
    hbox->addWidget(qLabel);
    hbox->addStretch();
    qFrame->setLayout(hbox);

    randomCode();

    
    QPair<QPixmap*, QPainter*> pixmapPainter = getEnterIDCodePixmapPainter();
    QPixmap* pixmap = pixmapPainter.first;
    QPainter* painter = pixmapPainter.second;
    delete painter;
    qLabel->setPixmap(*pixmap);

    if(currEnterIDCodePixmap) {
        delete currEnterIDCodePixmap;
        currEnterIDCodePixmap = nullptr;
    }
    currEnterIDCodePixmap = pixmap;

    if(currEnterIDCodeLabel) {
        delete currEnterIDCodeLabel;
        currEnterIDCodeLabel = nullptr;
    }
    currEnterIDCodeLabel = qLabel;

    return qFrame;

} 

void onCloseEnterIDCode() {
    playSound("Fix_Wiring_task_close_sound.wav");
    resetCode();
    if(currEnterIDCodeLabel) {
        delete currEnterIDCodeLabel;
        currEnterIDCodeLabel = nullptr;
    }
    if(currEnterIDCodePixmap) {
        delete currEnterIDCodePixmap;
        currEnterIDCodePixmap = nullptr;
    }
}

void resetCode() {
    for(quint8 i = 0; i< DIGIT_NUMBER; i++){
        code[i] = DIGIT_UNDEFINED;
    }
}

