#include <GameWindow.h>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <cmath>
#include <QPixmap>
#include <QDebug>
GameWindow::GameWindow(QWidget* parent) :
    QMainWindow(parent)
{

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &GameWindow::updateGame);
    Vx = 0.0;
    Vy = 0.0;
    timer->start(20); // 50 FPS
    imageJoueur = QPixmap(":/Sujet1Math/voiture.png");
}

GameWindow::~GameWindow()
{
}

void GameWindow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::white);


    ScreenPoint PlayerScreen = toScreen(PlayerPoint);


    double angle = std::atan2(Vy, Vx) * 180.0 / M_PI;

    painter.save();

    painter.translate(PlayerScreen.first, PlayerScreen.second);

 
    painter.rotate(-angle);

  
    painter.drawPixmap( -50,-30, 100,60,imageJoueur);

    painter.restore();

 
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 20, QFont::Bold));

    std::string txt = "vitesse x : " + std::to_string(Vx) +
        " m/s \nvitesse y : " + std::to_string(Vy) + " m/s";

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
    double force = 0.0;

    if (rightPressed)
        force += 100.0;


    if (leftPressed)
        force -= 100.0;

    return force + frottementX();
}
double GameWindow::F_y() const
{
    double force = 0.0;

    if (upPressed)
        force += 100.0;

    if (downPressed)
        force -= 100.0;
    return (force - 10.0) + frottementY();
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


double GameWindow::frottementX() const
{
    double k = 1.8;
    return -k * Vx;
}

double GameWindow::frottementY() const
{
    double k = 1.8;
    return -k * Vy;
}