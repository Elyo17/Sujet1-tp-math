#include "PlayerEntity.h"
#include <QPainter>
#include <cmath>

// GameWindow::keyPressEvent va transmettre CHAQUE touche pressée
// Chaque PlayerEntity compare alors la touche reçue à SES PROPRES keys pour savoir si ça la concerne
// Si un joueur reçoit une touche qui n'est pas la sienne, aucun des "if" ne matche : rien ne se passe. 
// C'est ce qui permet à 2 PlayerEntity de coexister sans se marcher dessus.
void PlayerEntity::onKeyPress(int key)
{
    if (key == keys.up)    upPressed = true;
    if (key == keys.down)  downPressed = true;
    if (key == keys.left)  leftPressed = true;
    if (key == keys.right) rightPressed = true;
}

void PlayerEntity::onKeyRelease(int key)
{
    if (key == keys.up)    upPressed = false;
    if (key == keys.down)  downPressed = false;
    if (key == keys.left)  leftPressed = false;
    if (key == keys.right) rightPressed = false;
}


// EXACTEMENT la même logique que dans l'ancien GameWindow.cpp, simplement recopiée telle quelle.
// Dépend que de l'état interne du joueur (upPressed, Vx, Vy...), pas de GameWindow. 
// C'est le signe que cette logique était "à sa place" naturelle dans PlayerEntity depuis le début.
double PlayerEntity::frottementX() const { return -k * Vx; }
double PlayerEntity::frottementY() const { return -k * Vy; }

double PlayerEntity::F_x() const
{
    double force = 0.0;
    if (rightPressed) force += 100.0;
    if (leftPressed)  force -= 100.0;
    return force + frottementX();
}

double PlayerEntity::F_y() const
{
    double force = 0.0;
    if (upPressed)   force += 100.0;
    if (downPressed) force -= 100.0;
    return (force - 10.0) + frottementY();
}


// Différence avec l'original : "dt" est maintenant un PARAMETRE, (passé par update()) plutôt qu'une constante 0.02 codée en dur.
// Avantage : si un jour le timer change de fréquence, ou si on veut un dt variable, ce code n'a pas besoin de changer.
void PlayerEntity::calculateSpeed(double dt)
{
    Vx = (F_x() * dt) / mass + Vx;
    Vy = (F_y() * dt) / mass + Vy;
}


// Même logique que l'originale, MAIS utilise "minBound"/"maxBound"
// (fournis par GameWindow via setBounds()) au lieu d'appeler
// toPhysical() elle-même -- puisque PlayerEntity ne connaît pas
// GameWindow.
void PlayerEntity::rebond()
{
    if (position.first < minBound.first)
    {
        position.first = minBound.first;
        Vx = -Vx;
    }
    if (position.first > maxBound.first)
    {
        position.first = maxBound.first;
        Vx = -Vx;
    }
    if (position.second < minBound.second)
    {
        position.second = minBound.second;
        Vy = -Vy;
    }
    if (position.second > maxBound.second)
    {
        position.second = maxBound.second;
        Vy = -Vy;
    }
}


// Rassemble, dans l'ordre, exactement ce que faisait GameWindow::updateGame() pour la partie "joueur" :
// calcul de vitesse -> intégration de la position -> rebond.
// "position" est le membre PROTECTED hérité d'Entity (d'où l'intérêt de l'avoir mis en "protected" et pas "private").
void PlayerEntity::update(double dt)
{
    calculateSpeed(dt);
    position.first += Vx * dt;
    position.second += Vy * dt;
    rebond();
}


// Reprend paintEvent() pour la partie "dessin du joueur" : translation + rotation selon l'angle de déplacement + sprite.
// Ne dessine PAS le texte de debug (vitesse) : ce sera le rôle
// de GameWindow::paintEvent() de l'afficher, en lisant getVx()/
// getVy() -- car ce texte est une info de "debug global", pas
// vraiment une responsabilité propre à l'entité elle-même.
void PlayerEntity::draw(QPainter& painter, int screenX, int screenY)
{
    double angle = std::atan2(Vy, Vx) * 180.0 / M_PI;

    painter.save();
    painter.translate(screenX, screenY);
    painter.rotate(-angle);
    painter.drawPixmap(-50, -30, 100, 60, sprite);
    painter.restore();
}