#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QMainWindow>
using ScreenPoint = std::pair<int, int>;
using WorldPoint = std::pair<double, double>;

class GameWindow: public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    virtual ~GameWindow();

    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    ScreenPoint toScreen(const WorldPoint& point) const;
    WorldPoint toPhysical(const ScreenPoint& point) const;
    double _zoom = 50.0;
};

#endif

