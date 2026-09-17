#include <GameWindow.h>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <cmath>

GameWindow::GameWindow(QWidget* parent) :
    QMainWindow(parent)
{

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &GameWindow::updateGame);

    timer->start(20); // 50 FPS
}

GameWindow::~GameWindow()
{
}

void GameWindow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    // Clear background with White color.
    painter.fillRect(rect(), Qt::white);

    // Draw player as a yellow circle.
    int radius = 10;

    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::NoPen);
    ScreenPoint PlayerScreen = toScreen(PlayerPoint);
    
    painter.drawEllipse(PlayerScreen.first, PlayerScreen.second, radius, radius);

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 20, QFont::Bold));
    std::string txt = "vitesse x : " + std::to_string(Vx) + " m/s\nvitesse y : " + std::to_string(Vy) + " m/s";
    painter.drawText(50, 50, QString::fromStdString(txt));

}

void GameWindow::updateGame() 
{
    PreviousPlayerPoint = PlayerPoint;
   /* if (upPressed)
        PlayerPoint.second += 0.1;

    if (downPressed)
        PlayerPoint.second -= 0.1;

    if (rightPressed)
        PlayerPoint.first += 0.1;

    if (leftPressed)
        PlayerPoint.first -= 0.1;*/
    calculateSpeed();

    PlayerPoint.first = PlayerPoint.first + Vx * 0.02;
    PlayerPoint.second = PlayerPoint.second + Vy * 0.02;


    update();
}

void GameWindow::calculateSpeed()
{
    Vx = (F_x() * 0.02) / 1208 + Vx;
    Vy = (F_y() * 0.02) / 1208 + Vy;

}

double GameWindow::F_x() const
{
    return 0.0;
}
double GameWindow::F_y() const
{
    return -10.0;
}

void GameWindow::keyPressEvent(QKeyEvent* event)
{        
    if (event->key() == Qt::Key_Up)        upPressed = true;
    if (event->key() == Qt::Key_Down)        downPressed = true;
    if (event->key() == Qt::Key_Right)        rightPressed = true;
    if (event->key() == Qt::Key_Left)        leftPressed = true;
    QWidget::keyPressEvent(event);
}
void GameWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Up)        upPressed = false;
    if (event->key() == Qt::Key_Down)        downPressed = false;
    if (event->key() == Qt::Key_Right)        rightPressed = false;
    if (event->key() == Qt::Key_Left)        leftPressed = false;
    QWidget::keyReleaseEvent(event);
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

