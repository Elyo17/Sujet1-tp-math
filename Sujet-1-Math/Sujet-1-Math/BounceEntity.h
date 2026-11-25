#pragma once
#ifndef BOUNCE_ENTITY_H
#define BOUNCE_ENTITY_H

#include "Entity.h"
#include <QColor>


// C'est la classe fille la plus simple possible : elle montre qu'on peut "satisfaire le contrat" d'Entity (fournir update()
// et draw()) même quand update() ne fait littéralement rien.
// Comme elle fournit une implémentation concrète pour LES DEUX
// méthodes virtuelles pures d'Entity, FixedEntity n'est PAS
// abstraite : on peut créer des objets FixedEntity normalement.
class BounceEntity : public Entity
{
public:
    // On donne une position, une taille (largeur/hauteur en unités
    // "monde", pas en pixels), et une couleur pour la dessiner.
    BounceEntity(WorldPoint position, double w, double h, QColor color)
        : Entity(position, 0.5)   // appelle le constructeur de la classe
        // mère Entity pour initialiser "position"
        , width(w)
        , height(h)
        , color(color)
    {
    }

    // "override" : mot-clé qui dit explicitement au compilateur
    // "je redéfinis bien une méthode virtuelle de la classe mère".
    // Si jamais tu fais une faute de frappe dans la signature
    // (mauvais type, mauvais nom), le compilateur te préviendra
    // au lieu de créer silencieusement une NOUVELLE méthode
    // sans lien avec celle d'Entity. Toujours mettre "override".
    void update(double dt) override
    {
        // Volontairement vide : une entité fixe ne bouge jamais.
        // On garde quand même la fonction (obligatoire, car
        // virtuelle pure dans Entity) mais son corps ne fait rien.
        (void)dt; // évite un warning "paramètre non utilisé"
    }

    void draw(QPainter& painter, int screenX, int screenY)  override;
    // -> implémentation dans le .cpp, car elle utilise QPainter
    //    (dessin), qu'on préfère ne pas mettre dans le header.

    bool isSegment() const override { return true; }

    std::pair<double, double> pointA() const override
    {
        return { position.first - std::cos(angle) * width / 2, position.second - std::sin(angle) * width / 2 };
    }
    std::pair<double, double> pointB() const override
    {
        return { position.first + std::cos(angle) * width / 2, position.second + std::sin(angle) * width / 2 };
    }

private:
    double width;
    double height;
    QColor color;
    double angle = 0.0;
};

#endif