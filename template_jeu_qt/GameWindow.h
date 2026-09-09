#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QMainWindow>

class GameWindow: public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    virtual ~GameWindow();

    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif
