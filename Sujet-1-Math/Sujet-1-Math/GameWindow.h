#pragma once
#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QMainWindow>
#include <memory>
#include <vector>
#include <string>
#include "Entity.h"

using ScreenPoint = std::pair<int, int>;

class PlayerEntity;
class TrainingEntity;
class QTimer;

// ============================================================
// Les 4 modes du programme (section 6.6).
// On choisit le mode avec la constante MODE en haut de GameWindow.cpp.
// ============================================================
enum class GameMode
{
    Jeu,            // le jeu "normal" des sections précédentes
    Enregistrement, // Q1 : tu joues, ta trajectoire est sauvegardée
    Entrainement,   // Q2-Q3 : le fantôme rejoue, tu pilotes l'entité d'entraînement
    IA,             // Q4-Q7 : le fantôme rejoue, les ennemis IA le poursuivent
    Test            // tests automatiques de toute la 6.6 (un robot joue à ta place)
};

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameWindow(QWidget* parent = nullptr);
    virtual ~GameWindow();

    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    ScreenPoint toScreen(const WorldPoint& point) const;
    WorldPoint toPhysical(const ScreenPoint& point) const;
    void updateGame();
    void handleCollisions();
    void calculateSpeed();   // (déclarations de ta version d'origine, gardées telles quelles)
    double F_x() const;
    double F_y() const;

    void setupJeu();
    void setupEnregistrement();
    void setupEntrainement();
    void setupIA();
    void setupTest();

    double _zoom = 25.0;
    QTimer* timer;

    std::vector<std::unique_ptr<Entity>> entities;

    // Mode Enregistrement : le joueur suivi et ses positions
    PlayerEntity* recordedPlayer = nullptr;
    std::vector<WorldPoint> recordedTrajectory;

    // Mode Entrainement : l'entité que tu pilotes
    TrainingEntity* trainee = nullptr;

    // Mode Test : les lignes de résultat à afficher
    std::vector<std::string> testLines;
};

#endif
