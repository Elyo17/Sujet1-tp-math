#pragma once
#ifndef PATROL_ENTITY_H
#define PATROL_ENTITY_H

#include "Entity.h"
#include <QColor>


// Différence essentielle avec PlayerEntity :
//   - PlayerEntity  = mouvement "dynamique" (piloté par des
//                      forces qu'on intègre pour obtenir une
//                      vitesse, puis une position)
//   - PatrolEntity  = mouvement "cinématique" (on IMPOSE
//                      directement une vitesse constante, sans
//                      passer par des forces)
// Ce sont deux façons différentes de faire bouger une entité,
// d'où deux classes filles distinctes d'Entity.
class PatrolEntity : public Entity
{
public:
    // pointA / pointB : les deux extrémités du trajet.
    // speed : vitesse de déplacement constante (unités/seconde).
    PatrolEntity(WorldPoint pointA, WorldPoint pointB, double speed,
        double w, double h, QColor color)
        : Entity(pointA)   // on démarre sur pointA
        , start(pointA)
        , end(pointB)
        , speed(speed)
        , width(w)
        , height(h)
        , color(color)
    {
    }

    void update(double dt) override;
    void draw(QPainter& painter, int screenX, int screenY) override;

    // Pas besoin de redéfinir onKeyPress/onKeyRelease : le
    // comportement vide hérité d'Entity convient parfaitement,
    // un patrouilleur ignore totalement le clavier.

private:
    WorldPoint start;
    WorldPoint end;
    double speed;
    bool goingToEnd = true; // sens de déplacement actuel

    double width;
    double height;
    QColor color;
};

#endif