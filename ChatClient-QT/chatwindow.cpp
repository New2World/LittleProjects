#include "chatwindow.h"

chatwindow::chatwindow(QString name, QWidget *parent) : QWidget(parent), peerName(name){
    setChatLayout(name);
    setAttribute(Qt::WA_DeleteOnClose);
}

chatwindow::~chatwindow(){
    emit closed(peerName);
}

void chatwindow::setChatLayout(QString name){
    setWindowTitle(QString("Chatting with ").append(name));
    history = new QLabel("History");
    message = new QLabel("Input");
    output = new QTextBrowser();
    input = new QTextEdit();
    output->setReadOnly(true);
    output->setMinimumHeight(300);
    output->setMinimumWidth(400);
    input->setMinimumHeight(100);
    input->setMinimumWidth(400);
    send = new QPushButton("Send");
    hLay = new QHBoxLayout();
    hLay->addWidget(send);
    hLay->setAlignment(Qt::AlignRight);
    vLay = new QVBoxLayout();
    vLay->addWidget(history);
    vLay->addWidget(output);
    vLay->addWidget(message);
    vLay->addWidget(input);
    vLay->addLayout(hLay);
    setLayout(vLay);
    connect(send, SIGNAL(clicked(bool)), this, SLOT(sendToSock()));
}

void chatwindow::sendToSock(){
    QString msg = input->toPlainText();
    if(msg.isEmpty())   return;
    output->append("Me:"+msg);
    emit sendMsg(peerName+">"+msg);
    input->clear();
}

void chatwindow::fetchRcd(QStringList record){
    if(record.isEmpty())    return;
    foreach(QString msg, record){
        if(msg[0] == '#'){
            QString contain, name = firstBound(msg, contain);
            showOnScreen(name, contain);
        }else   showOnScreen(peerName, msg);
    }
    emit finished(peerName);
}

void chatwindow::showOnScreen(QString sender, QString msg){
    if(sender == peerName)  output->append(peerName+':'+msg);
    else if(sender[0] == '#' && peerName == "ALL")  output->append(QString(sender.mid(1))+':'+msg);
}

QString chatwindow::firstBound(QString src, QString& msg){
    int i = -1;
    while(src[++i] != '>');
    msg = src.mid(i+1);
    return src.mid(0, i);
}
