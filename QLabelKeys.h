#ifndef QLABELKEYS_H
#define QLABELKEYS_H

#include <QLabel>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QHBoxLayout>
#include <QIcon>
#include <QtMath>
#include <QRandomGenerator>
#include <QTimer>
#include <QElapsedTimer>
#include "fixWiring.h"
#include "qPlus.h"

const int FPS = 30;

class QLabelKeys : public QLabel
{
    Q_OBJECT

    private:
        quint16 x, y;
        int topBackground, leftBackground;
        QPixmap* playerPixmap;
        QPixmap* backgroundPixmap;
        QPixmap* collisionPixmap;
        QImage collisionImage;
        QPixmap* windowPixmap;
        QTimer* timer;
        QElapsedTimer* elapsedTimer;
        QMap<int, bool> isPressed;
        qint64 lastUpdate;
        QHBoxLayout* currLayout;

    public:
        QLabel* qLabel;
        void display();
        void displayAt(QPixmap *pixmap, int centerx, int centery);
        bool performMovement(qint64 elapsed, int dirVert, int dirHoriz);
        void setCenterBorderLimit(int x, int y);
        bool isCollision(quint16 x, quint16 y);
        void resizeEvent(QResizeEvent* ev);
        QLabelKeys(QLabel* parent = 0);

    public slots:
        void redraw();

    protected:
        bool eventFilter(QObject* obj, QEvent* event);
};

#endif
