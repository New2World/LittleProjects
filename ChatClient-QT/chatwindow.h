#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QTextBrowser>
#include <QTextEdit>
#include <QTcpSocket>
#include <headers.h>

class chatwindow : public QWidget{
    Q_OBJECT

    QString peerName;
    QLabel *history, *message;
    QTextBrowser *output;
    QTextEdit *input;
    QPushButton *send;
    QHBoxLayout *hLay;
    QVBoxLayout *vLay;

    void setChatLayout(QString);
    QString firstBound(QString, QString&);
public:
    explicit chatwindow(QString, QWidget *parent = 0);
    ~chatwindow();

signals:
    void closed(QString);
    void sendMsg(QString);
    void finished(QString);

public slots:
    void sendToSock();
    void fetchRcd(QStringList);
    void showOnScreen(QString, QString);

};

#endif // CHATWINDOW_H
