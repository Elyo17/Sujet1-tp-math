#pragma once
#ifndef ENEMY_ENTITY_H
#define ENEMY_ENTITY_H

#include "Entity.h"
#include <QColor>

// ============================================================
// EnemyEntity : un ennemi qui POURSUIT une cible (le joueur).
//
// l'ennemi a le droit de CONNAITRE la position
// de sa cible, mais il n'a PAS le droit de modifier directement sa
// position ni sa vitesse. Il ne peut agir que sur ses PROPULSIONS
// (= une force), exactement comme un joueur qui appuie sur les
// flèches. Ensuite, c'est la même physique que PlayerEntity :
//     force -> accélération (F/m) -> vitesse -> position
// ============================================================

// Les 3 stratégies de poursuite demandées par l'énoncé
enum class ChaseMode
{
    Regles,              // (1) "si la cible est à droite, j'appuie à droite"
    Proportionnel,       // (2) propulsion = Kp * distance
    ProportionnelAmorti  // (4) propulsion = Kp * distance + Kv * (Vcible - Vmoi)
};

// Tous les paramètres qui définissent un "profil" d'ennemi (question 7)
struct EnemyProfile
{
    double mass;       // masse (inertie)
    double k;          // coefficient de frottement
    double Kp;         // part de la DISTANCE à la cible dans la propulsion
    double Kv;         // part de la DIFFERENCE DE VITESSE dans la propulsion
    double maxThrust;  // propulsion max (un joueur pousse à 100)
    ChaseMode mode;
};

class EnemyEntity : public Entity
{
public:
    // target : l'entité à poursuivre (en pratique un PlayerEntity).
    // On stocke un pointeur NON propriétaire : c'est GameWindow
    // (via le vector de unique_ptr) qui possède les entités.
    EnemyEntity(WorldPoint startPosition, const Entity* target,
        EnemyProfile profile, QColor color)
        : Entity(startPosition, 0.4)
        , target(target)
        , p(profile)
        , color(color)
    {
    }

    void update(double dt) override;
    void draw(QPainter& painter, int screenX, int screenY) override;

    double getVx() const { return Vx; }
    double getVy() const { return Vy; }

    // Changer de cible (ex : viser le joueur le plus proche).
    // On réinitialise l'estimation de vitesse, sinon le saut de
    // position entre l'ancienne et la nouvelle cible serait vu
    // comme une vitesse énorme pendant une frame.
    void setTarget(const Entity* newTarget)
    {
        if (newTarget != target)
        {
            target = newTarget;
            firstUpdate = true;
        }
    }

    // Mêmes limites d'écran que PlayerEntity (fournies par GameWindow)
    void setBounds(WorldPoint minB, WorldPoint maxB)
    {
        minBound = minB;
        maxBound = maxB;
        boundsSet = true;
    }

private:
    WorldPoint minBound{ 0, 0 };
    WorldPoint maxBound{ 0, 0 };
    bool boundsSet = false;
    void rebond();

    const Entity* target;
    EnemyProfile p;
    QColor color;

    double Vx = 0.0;
    double Vy = 0.0;

    // Pour estimer la vitesse de la cible (question 4) : on n'a que sa
    // POSITION, donc on la dérive nous-mêmes : V ≈ (pos - posPrecedente) / dt
    WorldPoint lastTargetPos{ 0.0, 0.0 };
    bool firstUpdate = true;

    // Calcule la propulsion (Fx, Fy) selon le mode choisi
    void computeThrust(double dx, double dy, double vtx, double vty,
        double& Fx, double& Fy) const;
};

#endif