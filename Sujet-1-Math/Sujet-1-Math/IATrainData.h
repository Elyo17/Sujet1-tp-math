#pragma once
#ifndef IA_TRAIN_DATA_H
#define IA_TRAIN_DATA_H

#include <vector>

// Un point d'entraînement :
//   _x = les variables d'entrée (ici dx, dy, vtx, vty, vx, vy)
//   _y = la valeur à prédire (ici px OU py)
struct IATrainData
{
    std::vector<double> _x;
    double              _y;
};

// Fonction fournie sur Moodle (définie dans computeHyperplanCoefficients.cpp).
// Renvoie les coefficients { c0, c1, ..., cN } de l'hyperplan
//     y = c0 + c1*x1 + c2*x2 + ... + cN*xN
// qui passe au mieux par les données (moindres carrés).
// Renvoie un vecteur VIDE s'il n'y a pas assez de données.
std::vector<double> computeHyperplanCoefficients(const std::vector<IATrainData>& data);

#endif
