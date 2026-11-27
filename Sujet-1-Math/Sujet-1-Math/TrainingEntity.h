#pragma once
#ifndef TRAINING_ENTITY_H
#define TRAINING_ENTITY_H

#include "Entity.h"
#include "IAConfig.h"
#include <QColor>
#include <vector>
#include <string>

// ============================================================
// Questions (2) et (3) : l'entité d'entraînement.
//
// C'est TOI qui la pilotes au clavier (flèches), pendant que
// ReplayEntity rejoue la trajectoire du joueur. Ton but :
// suivre et toucher le joueur fantôme au mieux.
//
// A chaque pas de temps, elle enregistre :
//   - la situation : dx, dy, vtx, vty, vx, vy
//   - ta décision  : px, py (les propulsions que tu appliques)
// ============================================================

struct TrainingSample
{
    IA::Features x; // { dx, dy, vtx, vty, vx, vy }
    double px;
    double py;
};

bool saveTrainingData(const std::string& filename, const std::vector<TrainingSample>& samples);
std::vector<TrainingSample> loadTrainingData(const std::string& filename);

class TrainingEntity : public Entity
{
public:
    TrainingEntity(WorldPoint start, const Entity* target, QColor color)
        : Entity(start, 0.4), target(target), color(color)
    {
    }

    void update(double dt) override;
    void draw(QPainter& painter, int screenX, int screenY) override;
    void onKeyPress(int key) override;
    void onKeyRelease(int key) override;

    const std::vector<TrainingSample>& getSamples() const { return samples; }

private:
    const Entity* target;
    QColor color;

    double Vx = 0.0;
    double Vy = 0.0;
    IA::TargetTracker tracker;

    bool up = false, down = false, left = false, right = false;

    std::vector<TrainingSample> samples;
};

#endif
