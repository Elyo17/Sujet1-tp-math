#pragma once
#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QMainWindow>
#include <memory> // C'est pour le std::unique_ptr
#include <vector>
#include "Entity.h" // GameWindow ne connaît QUE l'interface abstraite Entity
                    //Mais pas PlayerEntity / FixedEntity / PatrolEntity ici : ces types concrets ne seront inclus que dans le .cpp,

using ScreenPoint = std::pair<int, int>;
    


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
    QTimer* timer;
   

    std::vector<std::unique_ptr<Entity>> entities;
};

#endif

