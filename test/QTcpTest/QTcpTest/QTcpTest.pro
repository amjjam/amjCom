QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.C \
    QTcpTest.C \
    /home/anders/nomad/packages/amjCom/src/amjCom.C \
    /home/anders/nomad/packages/amjCom/src/amjComASIO.C \
    /home/anders/nomad/packages/amjCom/src/amjComTCP.C


HEADERS += \
    QTcpTest.H

FORMS += \
    QTcpTest.ui

INCLUDEPATH += /home/anders/nomad/packages/amjCom/include
INCLUDEPATH += /opt/asio/include


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
