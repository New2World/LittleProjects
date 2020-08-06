#ifndef LOGIN_H
#define LOGIN_H

#include <QLineEdit>
#include <QTcpSocket>
#include <QMessageBox>
#include <headers.h>

class login : public QDialog{
    Q_OBJECT

    bool isVaild;
    QLabel *name, *pw;
    QLineEdit *usrName, *passwd;
    QPushButton *signin, *signup;
    QVBoxLayout *layout;
    QHBoxLayout *hLayout[3];
    QTcpSocket *tcp;
    QMessageBox *msgBox;

    void initLayout();
    void showDialog(QString);
    void sendUsrInfo();
public:
    explicit login(QDialog *parent = 0);
    virtual ~login();
    QString execute();
signals:
    void vaild();
private slots:
    void check();
    void sign();
    void log();
public slots:
};

#endif // LOGIN_H
