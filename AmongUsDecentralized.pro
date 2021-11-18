QT += widgets
QT += network
QT += multimedia
QMAKE_CXXFLAGS += -std=c++17
LIBS += \
   -lhelib \
   -lpthread \
   -lntl

SOURCES += \
    InGameUI.cpp \
    asteroids.cpp \
    Player.cpp \
    fixWiring.cpp \
    main.cpp \
    qPlus.cpp \
    Server.cpp \
    Client.cpp \
    Task.cpp \
    dummy_helib_test.cpp

HEADERS += \
    InGameUI.h \
    asteroids.h \
    Player.h \
    fixWiring.h \
    main.h \
    qPlus.h \
    Server.h \
    Client.h \
    Task.h


