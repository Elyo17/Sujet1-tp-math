#include <GameWindow.h>
#include <QKeyEvent>
#include <QPainter>

GameWindow::GameWindow(QWidget *parent):
    QMainWindow(parent)
{
}

GameWindow::~GameWindow()
{
}

void GameWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    // Clear background with White color.
    painter.fillRect(rect(), Qt::white);

    // Draw player as a yellow circle.
    int radius = 10;

    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(100 - radius, 100 - radius, radius, radius);
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        // Left key pressed.
        break;

    case Qt::Key_Right:
        // Right key pressed.
        break;

    case Qt::Key_Up:
        // Up key pressed.
        break;

    case Qt::Key_Down:
        // Down key pressed.
        break;

    default:
        QWidget::keyPressEvent(event);
        return;
    }
}
