#ifndef MAIN_H
#define MAIN_H

#include <QString>
#include <QMediaPlayer>
#include "InGameUI.h"
#include "Client.h"
#include "Server.h"

extern QMediaPlayer* player;
extern InGameUI* inGameUI;
enum InGameGUI{IN_GAME_GUI_NONE, IN_GAME_GUI_FIX_WIRING, IN_GAME_GUI_ASTEROIDS,IN_GAME_GUI_ENTER_ID_CODE};
extern InGameGUI currentInGameGUI;
extern QMap<Task,QVector<QPoint>> tasksLocations;
extern QList<Client*> clients;
extern Server* server;

#endif
