#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

#include <QMenu>
#include <QSystemTrayIcon>

#include "player.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    QPushButton *prev, *play, *next, *minimize, *reload;
    QHBoxLayout *layout;
    QVBoxLayout *funcButtons;

    QIcon indexIcon, trayIndexIcon;
    QIcon prevIcon, playIcon, pauseIcon, nextIcon;
    QIcon trayPrevIcon, trayPlayIcon, trayPauseIcon, trayNextIcon;

    QSystemTrayIcon *trayIcon;
    QAction *playAction, *prevAction, *nextAction, *showAction, *quitAction;
    QMenu *trayMenu;

    Player *player;

    void setupIcons();
    void setupWindow();
    void setupTray();
    void setupPlayer();
    void setupConnection();
private slots:
    void updatePlayAndPause();
};

#endif // WIDGET_H
