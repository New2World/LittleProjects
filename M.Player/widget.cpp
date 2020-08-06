#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    setupIcons();
    setupTray();
    setupWindow();
    setupPlayer();
    setupConnection();
}

void Widget::setupIcons(){
    prevIcon.addFile(tr(":/res/prev.png"));
    nextIcon.addFile(tr(":/res/next.png"));
    playIcon.addFile(tr(":/res/play.png"));
    pauseIcon.addFile(tr(":/res/pause.png"));
    indexIcon.addFile(tr(":/res/index.png"));
    trayPrevIcon.addFile(tr(":/res/tray_prev.png"));
    trayNextIcon.addFile(tr(":/res/tray_next.png"));
    trayPlayIcon.addFile(tr(":/res/tray_play.png"));
    trayPauseIcon.addFile(tr(":/res/tray_pause.png"));
    trayIndexIcon.addFile(tr(":/res/tray_index.png"));
}

void Widget::setupWindow(){
    prev = new QPushButton();
    next = new QPushButton();
    play = new QPushButton();

    prev->setIcon(prevIcon);
    prev->setFixedSize(45,45);
    prev->setIconSize(QSize(40,40));
    prev->setFlat(true);
    next->setIcon(nextIcon);
    next->setFixedSize(45,45);
    next->setIconSize(QSize(40,40));
    next->setFlat(true);
    play->setIcon(pauseIcon);
    play->setFixedSize(60,60);
    play->setIconSize(QSize(55,55));
    play->setFlat(true);

    minimize = new QPushButton("Mini");
    minimize->setFixedSize(50,30);
    minimize->setFlat(true);
    reload = new QPushButton("Reload");
    reload->setFixedSize(50,30);
    reload->setFlat(true);

    funcButtons = new QVBoxLayout();
    funcButtons->addWidget(minimize);
    funcButtons->addWidget(reload);

    layout = new QHBoxLayout();
    layout->addWidget(prev,2);
    layout->addWidget(play,3);
    layout->addWidget(next,2);
    layout->addLayout(funcButtons);

    this->setLayout(layout);
    this->setWindowTitle("M.Player");
    this->setWindowIcon(indexIcon);
}

void Widget::setupTray(){
    trayIcon = new QSystemTrayIcon();
    trayIcon->setIcon(trayIndexIcon);
    trayMenu = new QMenu();

    playAction = new QAction(trayPauseIcon, "Pause");
    prevAction = new QAction(trayPrevIcon, "Prev");
    nextAction = new QAction(trayNextIcon, "Next");
    showAction = new QAction("Show Panel");
    quitAction = new QAction("Quit");

    trayMenu->addAction(playAction);
    trayMenu->addAction(prevAction);
    trayMenu->addAction(nextAction);
    trayMenu->addSeparator();
    trayMenu->addAction(showAction);
    trayMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayMenu);
}

void Widget::setupPlayer(){
    player = new Player(this);
}

void Widget::setupConnection(){
    connect(minimize, SIGNAL(clicked(bool)), trayIcon, SLOT(show()));
    connect(minimize, SIGNAL(clicked(bool)), this, SLOT(hide()));

    connect(showAction, SIGNAL(triggered(bool)), this, SLOT(show()));
    connect(showAction, SIGNAL(triggered(bool)), trayIcon, SLOT(hide()));

    connect(play, SIGNAL(clicked(bool)), this, SLOT(updatePlayAndPause()));
    connect(playAction, SIGNAL(triggered(bool)), this, SLOT(updatePlayAndPause()));

    connect(quitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

    connect(playAction, SIGNAL(triggered(bool)), player, SLOT(playAndPauseMusic()));
    connect(play, SIGNAL(clicked(bool)), player, SLOT(playAndPauseMusic()));
    connect(nextAction, SIGNAL(triggered(bool)), player, SLOT(nextMusic()));
    connect(next, SIGNAL(clicked(bool)), player, SLOT(nextMusic()));
    connect(prevAction, SIGNAL(triggered(bool)), player, SLOT(prevMusic()));
    connect(prev, SIGNAL(clicked(bool)), player, SLOT(prevMusic()));

    connect(reload, SIGNAL(clicked(bool)), player, SLOT(reloadMusic()));
}

void Widget::updatePlayAndPause(){
    if(player->playing()){
        play->setIcon(playIcon);
        playAction->setIcon(trayPlayIcon);
        playAction->setText("Play");
    }else{
        play->setIcon(pauseIcon);
        playAction->setIcon(trayPauseIcon);
        playAction->setText("Pause");
    }
}

Widget::~Widget()
{
    delete prev;
    delete next;
    delete play;
    delete minimize;
    delete layout;
    delete playAction;
    delete prevAction;
    delete nextAction;
    delete showAction;
    delete quitAction;
    delete trayMenu;
    delete trayIcon;
    delete player;
    delete ui;
}
