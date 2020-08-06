#include "player.h"

Player::Player(QObject *parent) : QObject(parent){
    player = new QMediaPlayer();
    playlist = new QMediaPlaylist();
    addPlayList();
    player->play();
}

void Player::addPlayList(){
    QDir musicDir("/home/ritchie/Music/Playlist");
    musicDir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList infoList = musicDir.entryInfoList();
    int count = infoList.count();
    for(int i = 0;i < count;i++)
        playlist->addMedia(QUrl::fromLocalFile(infoList.at(i).absoluteFilePath()));
    playlist->setPlaybackMode(QMediaPlaylist::Random);
    player->setPlaylist(playlist);
    playlist->next();
}

void Player::playAndPauseMusic(){
    if(player->state() == QMediaPlayer::PlayingState)
        player->pause();
    else
        player->play();
}

void Player::nextMusic(){
    playlist->next();
}

void Player::prevMusic(){
    playlist->previous();
}

bool Player::playing(){
    return player->state() == QMediaPlayer::PlayingState;
}

void Player::reloadMusic(){
    player->stop();
    playlist->clear();
    addPlayList();
    player->play();
}

Player::~Player(){
    delete playlist;
    delete player;
}
