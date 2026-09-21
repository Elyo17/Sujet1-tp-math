#pragma once
#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QMainWindow>
using ScreenPoint = std::pair<int, int>;
using WorldPoint = std::pair<double, double>;


class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget* parent = nullptr);
    virtual ~GameWindow();

    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    ScreenPoint toScreen(const WorldPoint& point) const;
    WorldPoint toPhysical(const ScreenPoint& point) const;
    void updateGame();
    void calculateSpeed();
    double F_x() const;
    double F_y() const;
    double _zoom = 50.0;
    WorldPoint PlayerPoint = {0,0};
    WorldPoint PreviousPlayerPoint = {0,0};
    QTimer* timer;
    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
    float vitesse = 0;
    double Vx;
    double Vy;
    double k = 1.8;
    double frottementX() const;
    double frottementY() const;
    QPixmap imageJoueur;
    double angle = std::atan2(Vy, Vx);
};

#endif

