#include "BounceEntity.h"
#include <QPainter>

// draw() reçoit la position déjà convertie en pixels écran
// (screenX, screenY) par GameWindow -- FixedEntity n'a pas
// besoin de connaître le zoom ni la taille de la fenêtre.
void BounceEntity::draw(QPainter& painter, int screenX, int screenY)
{
    painter.save();
    painter.setBrush(color);
    painter.setPen(Qt::black);

    // On dessine un simple rectangle centré sur (screenX, screenY).
    // width/height sont ici en "pixels écran" pour simplifier le
    // dessin (on pourrait aussi les multiplier par le zoom si on
    // voulait qu'ils soient définis en unités monde -- à voir selon
    // vos besoins).
    double demi = width / 2.0;
    double dx = std::cos(angle) * demi;
    double dy = std::sin(angle) * demi;

    painter.drawLine(QPointF(screenX - dx, screenY - dy),QPointF(screenX + dx, screenY + dy));    
    painter.restore();
}