#include "login.h"

#define WAIT_FOR_CONNECTION 10000

login::login(QDialog *parent) : QDialog(parent){
    isVaild = false;
    tcp = new QTcpSocket();
    initLayout();
    connect(signin, SIGNAL(clicked(bool)), this, SLOT(log()));
    connect(signup, SIGNAL(clicked(bool)), this, SLOT(sign()));
    connect(tcp, SIGNAL(readyRead()), this, SLOT(check()));
}

login::~login(){
    tcp->close();
}

void login::initLayout(){
    name = new QLabel("User Name");
    pw = new QLabel("Password");
    name->setAlignment(Qt::AlignCenter);
    pw->setAlignment(Qt::AlignCenter);
    name->setMinimumWidth(80);
    pw->setMinimumWidth(80);
    usrName = new QLineEdit();
    passwd = new QLineEdit();
    passwd->setEchoMode(QLineEdit::Password);
    signin = new QPushButton("Login");
    signup = new QPushButton("Sign Up");
    msgBox = new QMessageBox();
    hLayout[0] = new QHBoxLayout();
    hLayout[0]->addWidget(name);
    hLayout[0]->addWidget(usrName);
    hLayout[1] = new QHBoxLayout();
    hLayout[1]->addWidget(pw);
    hLayout[1]->addWidget(passwd);
    hLayout[2] = new QHBoxLayout();
    hLayout[2]->addWidget(signin);
    hLayout[2]->addWidget(signup);
    layout = new QVBoxLayout();
    for(int i = 0;i < 3;i++)    layout->addLayout(hLayout[i]);
    setWindowTitle("Login");
    setLayout(layout);
}

QString login::execute(){
    this->exec();
    tcp->disconnectFromHost();
    if(isVaild) return usrName->text();
    return "";
}

void login::log(){
    if(tcp->state() == QAbstractSocket::ConnectedState) tcp->disconnectFromHost();
    tcp->connectToHost(AIM_IP, PORT_LOGIN);
    if(!(tcp->waitForConnected(WAIT_FOR_CONNECTION)))  showDialog("please check your network");
    else    sendUsrInfo();
}

void login::sign(){
    if(tcp->state() == QAbstractSocket::ConnectedState) tcp->disconnectFromHost();
    tcp->connectToHost(AIM_IP, PORT_SIGNUP);
    if(!(tcp->waitForConnected(WAIT_FOR_CONNECTION)))  showDialog("please check your network");
    else    sendUsrInfo();
}

void login::check(){
    char response[2];
    tcp->read(response, 2);
    if(tcp->peerPort() == PORT_LOGIN){
        if(response[0] == 'Y'){isVaild = true;this->close();}
        else    showDialog("invaild user or password\n");
    }else{
        if(response[0] == 'Y')  showDialog("sign up successfully");
        else    showDialog("user already exists");
    }
}

void login::showDialog(QString hint){
    msgBox->setWindowTitle("Warning");
    msgBox->setText(hint);
    msgBox->show();
}

void login::sendUsrInfo(){
    QString name = usrName->text(), pswd = passwd->text();
    if(name.isEmpty()){
        showDialog("user name cannot be empty");
        return;
    }else if(pswd.isEmpty()){
        showDialog("password cannot be empty");
        return;
    }
    name.append("&").append(pswd);
    tcp->write(name.toLatin1());
    tcp->flush();
}
