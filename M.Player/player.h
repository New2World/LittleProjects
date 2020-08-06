#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QDir>
#include <QStringList>
#include <QMediaPlayer>
#include <QMediaPlaylist>

class Player : public QObject
{
    Q_OBJECT
private:
    QMediaPlayer *player;
    QMediaPlaylist *playlist;

    void addPlayList();

public:
    explicit Player(QObject *parent = nullptr);
    ~Player();

    bool playing();

public slots:
    void playAndPauseMusic();
    void prevMusic();
    void nextMusic();
    void reloadMusic();
};

#endif // PLAYER_H
