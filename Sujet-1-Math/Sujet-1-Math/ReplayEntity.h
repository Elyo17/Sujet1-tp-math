#pragma once
#ifndef REPLAY_ENTITY_H
#define REPLAY_ENTITY_H

#include "Entity.h"
#include <QPixmap>
#include <vector>
#include <string>

// ============================================================
// Question (1) : joueur à trajectoire enregistrée.
//
// Partie "sauvegarde" : GameWindow (mode ENREGISTREMENT) note la
// position du joueur à chaque pas de temps, puis appelle
// saveTrajectory() à la fermeture de la fenêtre.
//
// Partie "rejeu" : ReplayEntity relit le fichier et reprend
// exactement une position par pas de temps. C'est un mouvement
// CINEMATIQUE (comme PatrolEntity) : on impose la position, on
// ne calcule aucune force. Arrivé au bout, on recommence.
// ============================================================

bool saveTrajectory(const std::string& filename, const std::vector<WorldPoint>& points);
std::vector<WorldPoint> loadTrajectory(const std::string& filename);

class ReplayEntity : public Entity
{
public:
    ReplayEntity(const std::vector<WorldPoint>& trajectory, QPixmap sprite);

    void update(double dt) override;
    void draw(QPainter& painter, int screenX, int screenY) override;

    bool isEmpty() const { return trajectory.empty(); }
    size_t size() const { return trajectory.size(); }
    size_t currentIndex() const { return index; }

private:
    std::vector<WorldPoint> trajectory;
    size_t index = 0;
    QPixmap sprite;
    double angle = 0.0; // pour orienter la voiture dans le sens du mouvement
};

#endif
