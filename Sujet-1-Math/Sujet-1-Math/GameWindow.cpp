#include <GameWindow.h>
#include <QKeyEvent>
#include <QPainter>

GameWindow::GameWindow(QWidget* parent) :
    QMainWindow(parent)
{
}

GameWindow::~GameWindow()
{
}

void GameWindow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    // Clear background with White color.
    painter.fillRect(rect(), Qt::white);
#ifdef _DEBUG // affichage des axes x et y en mode Debug
    painter.setPen(Qt::red);

    // Axe x
    painter.drawLine(0, height() / 2,width(), height() / 2);

    // Axe y
    painter.drawLine(width() / 2, 0,width() / 2, height());
#endif
    
    
    ScreenPoint position = toScreen({ position_player_x, position_player_y });
    // Draw player as a yellow circle.
    int radius = 10;

    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(position.first - radius,position.second - radius,2 * radius,2 * radius );
}

void GameWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
        position_player_x--;
       
        break;

    case Qt::Key_Right:
        position_player_x++;
        break;

    case Qt::Key_Up:
        position_player_y++;
        break;

    case Qt::Key_Down:
        position_player_y--;
        break;

    default:
        QWidget::keyPressEvent(event);
        return;
    }
    update();
}
ScreenPoint GameWindow::toScreen(const WorldPoint& point) const
{
    double x = point.first;
    double y = point.second;
    int X = static_cast<int>(width() / 2.0 + _zoom * x);
    int Y = static_cast<int>(height() / 2.0 - _zoom * y);
    return { X, Y };
}

WorldPoint GameWindow::toPhysical(const ScreenPoint& point) const
{
    double X = point.first;
    double Y = point.second;
    double x = (X - width() / 2.0) / _zoom;
    double y = (height() / 2.0 - Y) / _zoom;
    return { x, y };
}
