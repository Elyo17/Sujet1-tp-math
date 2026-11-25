#pragma once
#ifndef FOLLOWER_ENTITY_H
#define FOLLOWER_ENTITY_H

#include "Entity.h"
#include <QColor>

class FollowerEntity : public Entity
{
public:
    FollowerEntity(WorldPoint startPosition, const Entity* target, QColor color)
        : Entity(startPosition, 0.4)
        , target(target)
        , color(color)
    {
    }

    void update(double dt) override;
    void draw(QPainter& painter, int screenX, int screenY) override;

    // Question (3) : mettre 0.0 pour désactiver le second terme.
    // Question (6) : remettre 100.0 pour le réactiver.
    void setTangentialCoef(double c) { tangentialCoef = c; }

    double getVx() const { return Vx; }
    double getVy() const { return Vy; }

private:
    const Entity* target;
    QColor color;

    // Valeurs données par l'énoncé
    const double mass = 10.0;
    const double c = 3.0;          // frottement en v²
    double tangentialCoef = 100.0; // le "100" du second terme

    double Vx = 0.0;
    double Vy = 0.0;

    // Dernière propulsion calculée : on la garde pour pouvoir la
    // DESSINER (très pratique pour faire le schéma des questions 3 et 6)
    double lastPx = 0.0;
    double lastPy = 0.0;

    // Le code de l'énoncé, adapté à nos noms de variables
    double propulsion_x() const;
    double propulsion_y() const;
};

#endif