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
        if (other.isSegment())
        {
            // point A et B du segment
            auto A = other.pointA();
            auto B = other.pointB();

            double sx = B.first - A.first, sy = B.second - A.second;
            double px = position.first - A.first, py = position.second - A.second;

            // t = position du point le plus proche sur le segment (0 = A, 1 = B)
            double longueurCarree = sx * sx + sy * sy;
            double t = (longueurCarree > 0) ? (px * sx + py * sy) / longueurCarree : 0;
            if (t < 0) t = 0;
            if (t > 1) t = 1;

            // point le plus proche du centre, sur le segment
            double cx = A.first + t * sx;
            double cy = A.second + t * sy;

            double dx = position.first - cx;
            double dy = position.second - cy;
            // seul le rayon du cercle compte (le segment n'a pas de rayon)
            return dx * dx + dy * dy <= radius * radius;
        }

        // cas normal : deux cercles
        double dx = position.first - other.position.first;
        double dy = position.second - other.position.second;
        double distanceSquared = dx * dx + dy * dy;
        double radiusSum = radius + other.radius;
        return distanceSquared <= radiusSum * radiusSum;
    }
    
    virtual bool isSegment() const { return false; }
    virtual std::pair<double, double> pointA() const { return position; }
    virtual std::pair<double, double> pointB() const { return position; }

protected:
    WorldPoint position;
    double radius;
};

#endif