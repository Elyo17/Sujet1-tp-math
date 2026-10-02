#include "FixedEntity.h"
#include <QPainter>

// draw() reçoit la position déjà convertie en pixels écran
// (screenX, screenY) par GameWindow -- FixedEntity n'a pas
// besoin de connaître le zoom ni la taille de la fenêtre.
void FixedEntity::draw(QPainter& painter, int screenX, int screenY)
{
    painter.save();
    painter.setBrush(color);
    painter.setPen(Qt::black);

    // On dessine un simple rectangle centré sur (screenX, screenY).
    // width/height sont ici en "pixels écran" pour simplifier le
    // dessin (on pourrait aussi les multiplier par le zoom si on
    // voulait qu'ils soient définis en unités monde -- à voir selon
    // vos besoins).
    painter.drawRect(
        screenX - static_cast<int>(width / 2),
        screenY - static_cast<int>(height / 2),
        static_cast<int>(width),
        static_cast<int>(height)
    );

    painter.restore();
}