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
    explicit Entity(WorldPoint startPosition, double radius)
        : position(startPosition), radius(radius)
    {
    }

    virtual ~Entity() = default;
    virtual void update(double dt) = 0;
    virtual void draw(QPainter& painter, int screenX, int screenY) = 0;
    virtual void onKeyPress(int key) {}
    virtual void onKeyRelease(int key) {}
    WorldPoint getPosition() const { return position; }
    double getRadius() const { return radius; }
    bool isColliding(const Entity& other) const
    {
        // recupere la distance entre les 2 entitées
        double dx = position.first - other.position.first;
        double dy = position.second - other.position.second;
        double distanceSquared = dx * dx + dy * dy;
        // renvoie si les 2 rayon se touches
        double radiusSum = radius + other.radius;
        return distanceSquared <= radiusSum * radiusSum;
    }
protected:
    WorldPoint position;
    double radius;
};

#endif