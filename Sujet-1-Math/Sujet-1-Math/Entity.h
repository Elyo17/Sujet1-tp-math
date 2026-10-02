#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include <utility> // pour std::pair (utilisé par WorldPoint)

class QPainter; // "forward declaration" : on dit juste "cette classe existe"
// sans inclure tout QPainter.h ici, car on n'en a besoin
// que dans les signatures (paramètre par référence),
// pas dans le corps de code de ce header.

using WorldPoint = std::pair<double, double>;

class Entity
{
public:
    // Constructeur : toute entité a une position de départ.
    explicit Entity(WorldPoint startPosition)
        : position(startPosition)
    {
    }

    virtual ~Entity() = default;
    virtual void update(double dt) = 0;
    virtual void draw(QPainter& painter, int screenX, int screenY) = 0;
    virtual void onKeyPress(int key) {}
    virtual void onKeyRelease(int key) {}
    WorldPoint getPosition() const { return position; }

protected:
    WorldPoint position;
};

#endif