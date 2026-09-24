#pragma once
#ifndef PLAYER_ENTITY_H
#define PLAYER_ENTITY_H

#include "Entity.h"
#include <QPixmap>

//C'est ce qui permet d'avoir 2 joueur donc avoir les touches de flèche pour le joueur 1 et les touches ZQSD pour le 2ème joueur.
struct KeyBindings
{
    int up;
    int down;
    int left;
    int right;
};

// C'est la reprise de tout ce qui était directement dans GameWindow (Vx, Vy, F_x, F_y, frottement, rebond)
// Désormais encapsulé dans SA PROPRE classe, réutilisable
// Pour la création d'autant de joueurs qu'on veut.
class PlayerEntity : public Entity
{
public:
    PlayerEntity(WorldPoint startPosition, KeyBindings keys, QPixmap sprite)
        : Entity(startPosition,0.7)
        , keys(keys)
        , sprite(sprite)
    {
    }

    // --- Les 2 méthodes obligatoires (virtuelles pures dans Entity) ---
    void update(double dt) override;
    void draw(QPainter& painter, int screenX, int screenY) override;

    // --- Réaction au clavier : ICI on redéfinit vraiment,
    //     contrairement à FixedEntity qui n'y touche pas et
    //     garde le comportement vide hérité d'Entity. ---
    void onKeyPress(int key) override;
    void onKeyRelease(int key) override;

    // setBounds() : GameWindow calcule les limites physiques de
    // l'écran (via toPhysical, qui dépend de width()/height()/zoom
    // -- des choses que PlayerEntity ne doit PAS connaître) et les
    // transmet ici. PlayerEntity les stocke et les utilise dans
    // rebond(), appelé depuis update().
    void setBounds(WorldPoint minB, WorldPoint maxB)
    {
        minBound = minB;
        maxBound = maxB;
    }

    void ChangeCollider(bool c) 
    {
        collided = c;
    }

    // Utile pour l'affichage debug (texte "vitesse x : ...")
    double getVx() const { return Vx; }
    double getVy() const { return Vy; }

private:
    // --- Etat clavier : PROPRE à CE joueur (donc si joueur 1 et
    //     joueur 2 existent, chacun a ses propres booléens,
    //     aucun risque de conflit) ---
    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;

    // --- Touches associées à CE joueur ---
    KeyBindings keys;

    // --- Physique (identique à l'original) ---
    double Vx = 0.0;
    double Vy = 0.0;
    const double k = 1.8;        // coefficient de frottement
    const double mass = 1208.0;  // masse (comme dans calculateSpeed())

    // --- Limites de rebond, fournies par GameWindow ---
    WorldPoint minBound = { 0, 0 };
    WorldPoint maxBound = { 0, 0 };

    QPixmap sprite;

    // --- Méthodes privées : détails internes de la physique,
    //     inutiles pour le reste du programme, donc cachées ---
    double F_x() const;
    double F_y() const;
    double frottementX() const;
    double frottementY() const;
    void calculateSpeed(double dt);
    void rebond();

    // collision
    bool collided = false;
};

#endif