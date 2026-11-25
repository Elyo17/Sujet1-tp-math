#include "FollowerEntity.h"
#include <QPainter>
#include <cmath>

double FollowerEntity::propulsion_x() const
{
    if (!target)
        return 0.0;

    double dx = target->getPosition().first - position.first;
    double dy = target->getPosition().second - position.second;

    double d2 = dx * dx + dy * dy;
    double d = std::sqrt(d2);
    if (d < 1e-6)
        return 0.0; // sécurité : évite une division par zéro

    //terme radial (1er terme)       terme tangentiel (2nd terme)
    return (d2 - 625 / d2) * (dx / d) + tangentialCoef * dy / d;
}

double FollowerEntity::propulsion_y() const
{
    if (!target)
        return 0.0;

    double dx = target->getPosition().first - position.first;
    double dy = target->getPosition().second - position.second;

    double d2 = dx * dx + dy * dy;
    double d = std::sqrt(d2);
    if (d < 1e-6)
        return 0.0;

    return (d2 - 625 / d2) * (dy / d) - tangentialCoef * dx / d;
}

void FollowerEntity::update(double dt)
{
    lastPx = propulsion_x();
    lastPy = propulsion_y();

    // Frottement "en v²" : intensité c * v², direction opposée à la vitesse.
    // Vectoriellement : F = -c * |v| * v
    double v = std::sqrt(Vx * Vx + Vy * Vy);
    double Fx = lastPx - c * v * Vx;
    double Fy = lastPy - c * v * Vy;

    // Même intégration que PlayerEntity
    Vx += (Fx / mass) * dt;
    Vy += (Fy / mass) * dt;
    position.first += Vx * dt;
    position.second += Vy * dt;
}

void FollowerEntity::draw(QPainter& painter, int screenX, int screenY)
{
    painter.save();
    painter.setBrush(color);
    painter.setPen(Qt::black);
    painter.drawEllipse(screenX - 10, screenY - 10, 20, 20);

    // Dessin du VECTEUR PROPULSION (en bleu), pour observer sa direction.
    // Le "-" sur y : à l'écran, y est vers le bas (cf. toScreen).
    double scale = 0.5; // 1 unité de force = 0.5 pixel (à ajuster)
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawLine(screenX, screenY,
        screenX + static_cast<int>(scale * lastPx),
        screenY - static_cast<int>(scale * lastPy));
    painter.restore();
}