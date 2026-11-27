#pragma once
#ifndef IA_CONFIG_H
#define IA_CONFIG_H

#include "Entity.h"
#include <array>
#include <cmath>

// ============================================================
// Réglages COMMUNS à l'entité d'entraînement et à l'ennemi IA.
//
// TRES IMPORTANT : l'ennemi IA doit avoir EXACTEMENT la même
// physique que l'entité que tu as pilotée pendant l'entraînement.
// Les coefficients appris disent "dans telle situation, pousse
// de tant" : si la masse ou le frottement changent, la même
// poussée ne produit plus le même mouvement, et l'imitation
// ne marche plus.
// ============================================================
namespace IA
{
    const double MASS = 5.0;        // masse
    const double K = 4.0;           // frottement linéaire : F = -K * v
    const double MAX_THRUST = 20.0; // poussée quand on appuie sur une touche
                                    // -> vitesse max = MAX_THRUST / K = 5 m/s

    // Les 6 variables d'entrée, TOUJOURS dans cet ordre :
    //   { dx, dy, vtx, vty, vx, vy }
    // -> coefficients appris : { c0, c1(dx), c2(dy), c3(vtx), c4(vty), c5(vx), c6(vy) }
    using Features = std::array<double, 6>;

    // Estime la vitesse de la cible à partir de ses positions successives :
    //   v ≈ (position - position précédente) / dt
    // Utilisé à l'IDENTIQUE à l'entraînement et par l'IA, pour que les
    // données aient le même sens dans les deux cas.
    struct TargetTracker
    {
        WorldPoint last{ 0.0, 0.0 };
        bool first = true;

        void measure(const WorldPoint& targetPos, double dt, double& vtx, double& vty)
        {
            if (first)
            {
                last = targetPos;
                first = false;
            }
            vtx = (targetPos.first - last.first) / dt;
            vty = (targetPos.second - last.second) / dt;
            last = targetPos;
        }
    };

    // Un pas de physique (même schéma que PlayerEntity) :
    // force -> vitesse -> position
    inline void step(WorldPoint& pos, double& vx, double& vy,
                     double px, double py, double dt)
    {
        double Fx = px - K * vx;
        double Fy = py - K * vy;
        vx += (Fx / MASS) * dt;
        vy += (Fy / MASS) * dt;
        pos.first += vx * dt;
        pos.second += vy * dt;
    }

    // Noms de fichiers par défaut (créés dans le dossier du projet)
    const char* const TRAJECTORY_FILE = "trajectoire.txt";
    const char* const TRAINING_FILE = "entrainement.csv";
}

#endif
