#include "PatrolEntity.h"
#include <QPainter>
#include <cmath>

void PatrolEntity::update(double dt)
{
    // La cible actuelle dépend du sens de déplacement.
    WorldPoint target = goingToEnd ? end : start;

    // Vecteur allant de la position actuelle vers la cible.
    double dx = target.first - position.first;
    double dy = target.second - position.second;
    double distanceToTarget = std::sqrt(dx * dx + dy * dy);

    // Distance qu'on s'apprête à parcourir pendant ce pas de temps.
    double step = speed * dt;

    if (distanceToTarget <= step)
    {
        // On serait arrivé (ou on aurait dépassé) la cible pendant ce pas de temps : on se cale PILE sur la cible.
        // C'est réaliser pour ne pas "trembler" autour du point, puis on inverse le sens pour le prochain appel à update().
        position = target;
        goingToEnd = !goingToEnd;
    }
    else
    {
        // Sinon on avance simplement de "step" en direction de la cible : on normalise le vecteur (dx, dy).
        //Puis (on le ramène à une longueur de 1) puis on le multiplie par "step".
        double normX = dx / distanceToTarget;
        double normY = dy / distanceToTarget;

        position.first += normX * step;
        position.second += normY * step;
    }
}

void PatrolEntity::draw(QPainter& painter, int screenX, int screenY)
{
    painter.save();
    painter.setBrush(color);
    painter.setPen(Qt::black);
    painter.drawEllipse(
        screenX - static_cast<int>(width / 2),
        screenY - static_cast<int>(height / 2),
        static_cast<int>(width),
        static_cast<int>(height)
    );
    painter.restore();
}