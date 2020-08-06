#include "widget.h"
#include "ui_widget.h"

#define NAME_LEN 20
#define MAX_LEN 1000

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget){
    ui->setupUi(this);
    if((usrName = (new login())->execute()).isEmpty())   exit(0);
    setupPanel();
}

Widget::~Widget(){
    updateSock->write(QByteArray("-").append(usrName.toLatin1()));
    updateSock->flush();
    updateSock->disconnectFromHost();
    updateSock->close();
    delete ui;
}

void Widget::setupPanel(){
    vLay = new QVBoxLayout();
    usrList = new QListWidget();
    ALLItem = new QListWidgetItem("Group Chat");
    friendItems.insert(ALLItem, "ALL");
    usrList->addItem(ALLItem);
    vLay->addWidget(usrList);
    setLayout(vLay);
    setFixedSize(270, 450);
    setWindowTitle(usrName);
    updateSock = new QTcpSocket();
    updateSock->connectToHost(AIM_IP, PORT_PANEL);
    chatSock = new QTcpSocket();
    chatSock->connectToHost(AIM_IP, PORT_CHAT);
    connect(updateSock, SIGNAL(connected()), this, SLOT(sendName()));
    connect(updateSock, SIGNAL(readyRead()), this, SLOT(updateOnline()));
    connect(chatSock, SIGNAL(connected()), this, SLOT(sendName2()));
    connect(chatSock, SIGNAL(readyRead()), this, SLOT(storeRcd()));
    connect(usrList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openChatWindow(QListWidgetItem *)));
}

void Widget::sendName(){updateSock->write(QByteArray("+").append(usrName.toLatin1()));}

void Widget::sendName2(){chatSock->write(usrName.toLatin1());}

void Widget::updateOnline(){
    char logInfo[NAME_LEN];
    while(updateSock->read(logInfo, NAME_LEN) > 0){
        QString changedName = QString(logInfo+1);
        if(logInfo[0] == '+'){
            QListWidgetItem *tempItem = new QListWidgetItem(changedName);
            friendItems.insert(tempItem, changedName);
            usrList->addItem(tempItem);
        }else if(logInfo[0] == '-'){
            QListWidgetItem *tempItem = friendItems.keys(changedName)[0];
            usrList->removeItemWidget(tempItem);
            friendItems.remove(tempItem);
            delete tempItem;
        }
    }
}

void Widget::openChatWindow(QListWidgetItem *frd){
    QString name = friendItems[frd];
    if(openedList.contains(name))    return;
    if(frd->text().startsWith(">>> "))  frd->setText(name);
    openedList.append(name);
    chatwindow *chatWindow = new chatwindow(name);
    connect(this, SIGNAL(pushRcd(QStringList)), chatWindow, SLOT(fetchRcd(QStringList)));
    connect(chatWindow, SIGNAL(closed(QString)), this, SLOT(removeName(QString)));
    connect(chatWindow, SIGNAL(sendMsg(QString)), this, SLOT(sendMsg(QString)));
    connect(chatWindow, SIGNAL(finished(QString)), this, SLOT(clearRcd(QString)));
    connect(this, SIGNAL(sendToWindow(QString, QString)), chatWindow, SLOT(showOnScreen(QString, QString)));
    connect(this, SIGNAL(closeAll()), chatWindow, SLOT(close()));
    chatWindow->show();
    emit pushRcd(recorder[name]);
}

void Widget::sendMsg(QString msg){
    chatSock->write(msg.toLatin1());
    chatSock->flush();
}

void Widget::recvMsg(QString sender, QString msg){emit sendToWindow(sender, msg);}

void Widget::clearRcd(QString name){recorder[name].clear();}

void Widget::storeRcd(){
    QString origin = chatSock->read(MAX_LEN), msg;
    QString sender = firstBound(origin, msg);
    if(openedList.contains(sender) || (sender[0] == '#' && openedList.contains("ALL"))) recvMsg(sender, msg);
    else{
        if(sender[0] != '#'){
            recorder[sender].append(msg);
            moveToTop(sender);
        }else   recorder["ALL"].append(origin);
    }
}

void Widget::moveToTop(QString name){
    QListWidgetItem *item = friendItems.keys(name)[0];
    usrList->removeItemWidget(item);
    delete item;
    item = new QListWidgetItem(">>> "+name+" <<<");
    usrList->insertItem(1, item);
}

void Widget::removeName(QString name){openedList.removeAt(openedList.indexOf(name));}

QString Widget::firstBound(QString src, QString& msg){
    int i = -1;
    while(src[++i] != '>');
    msg = src.mid(i+1);
    return src.mid(0, i);
}
