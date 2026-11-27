#pragma once
#ifndef IA_TESTS_H
#define IA_TESTS_H

#include "Entity.h"
#include <QColor>
#include <string>
#include <vector>

// ============================================================
// Tests automatiques de la section 6.6 (mode GameMode::Test).
//
// Un "robot" joue à ta place pendant l'entraînement, avec 3 niveaux
// (facile, moyen, difficile). On vérifie ensuite chaque question :
//   Q1 : la trajectoire se sauvegarde et se relit à l'identique
//   Q2-Q3 : l'entraînement produit bien des données, relues sans perte
//   Q4 : on obtient 7 coefficients pour px et 7 pour py
//   Q5-Q6 : l'ennemi IA suit le fantôme (distance moyenne, contact)
//   Q7 : les 3 niveaux donnent des IA de difficulté croissante
//
// Les fichiers de test ont le préfixe "test_" : ils n'écrasent JAMAIS
// tes vrais fichiers (trajectoire.txt, entrainement.csv).
// ============================================================

struct TestedAI
{
    std::string name;
    QColor color;
    std::vector<double> coefX;
    std::vector<double> coefY;
};

struct IATestReport
{
    std::vector<std::string> lines; // texte à afficher
    bool allOk = true;
    std::vector<WorldPoint> trajectory; // trajectoire de test (pour l'affichage)
    std::vector<TestedAI> ais;          // les 3 IA entraînées par le robot
};

IATestReport runIATests();

#endif
