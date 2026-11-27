#pragma once
#ifndef AI_ENEMY_ENTITY_H
#define AI_ENEMY_ENTITY_H

#include "Entity.h"
#include "IAConfig.h"
#include <QColor>
#include <vector>
#include <string>

// ============================================================
// Question (4) : à partir d'un fichier d'entraînement, calcule
// les coefficients des 2 hyperplans (un pour px, un pour py).
// Renvoie false si le fichier est absent ou trop petit.
// ============================================================
bool trainFromFile(const std::string& filename,
                   std::vector<double>& coefX, std::vector<double>& coefY);

// ============================================================
// Question (5) : l'ennemi IA.
// Même physique que TrainingEntity, mais au lieu du clavier,
// il calcule ses propulsions avec les hyperplans appris :
//   px = c0 + c1*dx + c2*dy + c3*vtx + c4*vty + c5*vx + c6*vy
//   py = c'0 + c'1*dx + ... + c'6*vy
// ============================================================
class AIEnemyEntity : public Entity
{
public:
    AIEnemyEntity(WorldPoint start, const Entity* target,
                  std::vector<double> coefX, std::vector<double> coefY,
                  QColor color, std::string name)
        : Entity(start, 0.4), target(target)
        , coefX(std::move(coefX)), coefY(std::move(coefY))
        , color(color), name(std::move(name))
    {
    }

    void update(double dt) override;
    void draw(QPainter& painter, int screenX, int screenY) override;

    // Question (6) : des mesures pour juger objectivement l'ennemi
    double averageDistance() const { return frames ? sumDistance / frames : 0.0; }
    double contactRatio() const { return frames ? double(contactFrames) / frames : 0.0; }
    const std::string& getName() const { return name; }

private:
    const Entity* target;
    std::vector<double> coefX; // 7 coefficients pour px
    std::vector<double> coefY; // 7 coefficients pour py
    QColor color;
    std::string name;

    double Vx = 0.0;
    double Vy = 0.0;
    IA::TargetTracker tracker;

    double sumDistance = 0.0;
    long frames = 0;
    long contactFrames = 0;

    static double evaluate(const std::vector<double>& c, const IA::Features& x);
};

#endif
