#ifndef WIDGET_H
#define WIDGET_H

#include <QMap>
#include <QList>
#include <QListWidget>
#include <chatwindow.h>
#include <login.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget{
    Q_OBJECT

    QString usrName;
    QStringList openedList;
    QMap<QListWidgetItem *, QString> friendItems;
    QMap<QString, QStringList> recorder;
    QListWidget *usrList;
    QListWidgetItem *ALLItem;
    QVBoxLayout *vLay;
    QTcpSocket *updateSock, *chatSock;

    void setupPanel();
    void recvMsg(QString, QString);
    void moveToTop(QString);
    QString firstBound(QString, QString&);
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;

signals:
    void sendToWindow(QString, QString);
    void pushRcd(QStringList);
    void closeAll();

private slots:
    void sendName();
    void sendName2();
    void removeName(QString);
    void clearRcd(QString);

    void sendMsg(QString);
    void storeRcd();
    void openChatWindow(QListWidgetItem *);
    void updateOnline();

};

#endif // WIDGET_H
