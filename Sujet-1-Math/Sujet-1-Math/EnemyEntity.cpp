#include "EnemyEntity.h"
#include <QPainter>
#include <cmath>

void EnemyEntity::computeThrust(double dx, double dy, double vtx, double vty,
    double& Fx, double& Fy) const
{
    switch (p.mode)
    {
    case ChaseMode::Regles:
        // (1) Suivi "basé sur des règles" : l'ennemi se comporte comme
        // un joueur avec un pavé directionnel. Sur chaque axe :
        //   cible à droite -> j'appuie à droite (+F), sinon à gauche (-F)
        // C'est du "tout ou rien" : la poussée ne dépend pas de la distance.
        Fx = (dx > 0) ? p.maxThrust : (dx < 0 ? -p.maxThrust : 0.0);
        Fy = (dy > 0) ? p.maxThrust : (dy < 0 ? -p.maxThrust : 0.0);
        break;

    case ChaseMode::Proportionnel:
        // (2) Propulsion proportionnelle à la distance, sur chaque axe.
        // Loin -> pousse fort ; proche -> pousse doucement.
        // Physiquement, c'est un RESSORT accroché à la cible : F = Kp * (xcible - x)
        Fx = p.Kp * dx;
        Fy = p.Kp * dy;
        break;

    case ChaseMode::ProportionnelAmorti:
        // (4) On ajoute un terme qui compare MA vitesse à celle de la cible.
        // Si je vais plus vite que la cible vers elle, (vtx - Vx) est
        // négatif -> ce terme FREINE. Proche de la cible avec une grande
        // vitesse, le freinage l'emporte sur l'attraction : on n'accélère
        // plus, on ralentit pour ne pas la dépasser.
        // (C'est un correcteur "PD" : Proportionnel + Dérivé.)
        Fx = p.Kp * dx + p.Kv * (vtx - Vx);
        Fy = p.Kp * dy + p.Kv * (vty - Vy);
        break;
    }

    // Un moteur a une puissance limitée : on borne la NORME de la
    // propulsion (sinon Kp * 10 m donnerait une force irréaliste).
    double norm = std::sqrt(Fx * Fx + Fy * Fy);
    if (norm > p.maxThrust && norm > 0.0)
    {
        Fx *= p.maxThrust / norm;
        Fy *= p.maxThrust / norm;
    }
}

void EnemyEntity::update(double dt)
{
    if (target == nullptr)
        return;

    // --- 1. Observer la cible (autorisé par l'énoncé) ---
    WorldPoint targetPos = target->getPosition();
    if (firstUpdate)
    {
        lastTargetPos = targetPos;
        firstUpdate = false;
    }
    double vtx = (targetPos.first - lastTargetPos.first) / dt;
    double vty = (targetPos.second - lastTargetPos.second) / dt;
    lastTargetPos = targetPos;

    double dx = targetPos.first - position.first;
    double dy = targetPos.second - position.second;

    // --- 2. Choisir la propulsion (le SEUL levier de l'ennemi) ---
    double Fx = 0.0, Fy = 0.0;
    computeThrust(dx, dy, vtx, vty, Fx, Fy);

    // --- 3. Ajouter le frottement (comme PlayerEntity) ---
    Fx += -p.k * Vx;
    Fy += -p.k * Vy;

    // --- 4. Intégration : même schéma que PlayerEntity::update ---
    Vx += (Fx / p.mass) * dt;
    Vy += (Fy / p.mass) * dt;
    position.first += Vx * dt;
    position.second += Vy * dt;

    rebond();
}

// Même logique que PlayerEntity::rebond() : l'ennemi ne sort plus de l'écran
void EnemyEntity::rebond()
{
    if (!boundsSet)
        return;
    if (position.first < minBound.first) { position.first = minBound.first;   Vx = -Vx; }
    if (position.first > maxBound.first) { position.first = maxBound.first;   Vx = -Vx; }
    if (position.second < minBound.second) { position.second = minBound.second; Vy = -Vy; }
    if (position.second > maxBound.second) { position.second = maxBound.second; Vy = -Vy; }
}

void EnemyEntity::draw(QPainter& painter, int screenX, int screenY)
{
    painter.save();
    painter.setBrush(color);
    painter.setPen(Qt::black);
    painter.drawEllipse(screenX - 12, screenY - 12, 24, 24);

    // Petit trait indiquant la direction de la vitesse (utile pour
    // observer les dépassements / oscillations en testant)
    double v = std::sqrt(Vx * Vx + Vy * Vy);
    if (v > 1e-6)
        painter.drawLine(screenX, screenY,
            screenX + static_cast<int>(20 * Vx / v),
            screenY - static_cast<int>(20 * Vy / v));
    painter.restore();
}