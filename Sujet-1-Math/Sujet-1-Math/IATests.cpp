#include "IATests.h"
#include "IAConfig.h"
#include "ReplayEntity.h"
#include "TrainingEntity.h"
#include "AIEnemyEntity.h"
#include <QDebug>
#include <cmath>
#include <random>
#include <sstream>
#include <iomanip>

namespace
{
    const double DT = 0.02;

    // Une trajectoire de test : une courbe de Lissajous lente (~1 m/s),
    // qui ressemble à un joueur qui tourne dans l'écran. 60 s = 3000 pas.
    std::vector<WorldPoint> makeTestTrajectory()
    {
        std::vector<WorldPoint> traj;
        for (int i = 0; i < 3000; ++i)
        {
            double t = i * DT;
            traj.push_back({ 4.0 * std::sin(0.25 * t), 3.0 * std::sin(0.37 * t) });
        }
        return traj;
    }

    // Le niveau du "robot" qui joue à ta place pendant l'entraînement
    struct BotLevel
    {
        const char* name;
        const char* file;
        QColor color;
        int reactionFrames;  // il change ses touches toutes les N images
        double randomness;   // probabilité d'appuyer au hasard
        bool anticipates;    // freine-t-il avant d'arriver ?
    };

    // Le robot appuie sur les flèches, exactement comme toi :
    // il n'a accès qu'à onKeyPress / onKeyRelease.
    std::vector<TrainingSample> botTraining(const BotLevel& level,
                                            const std::vector<WorldPoint>& traj,
                                            std::mt19937& rng)
    {
        ReplayEntity ghost(traj, QPixmap());
        TrainingEntity trainer({ 5.0, 4.0 }, &ghost, Qt::yellow);
        std::uniform_real_distribution<double> u(0.0, 1.0);

        double lastX = trainer.getPosition().first, lastY = trainer.getPosition().second;
        WorldPoint lastGhost = ghost.getPosition();

        for (int i = 0; i < 6000; ++i) // 2 minutes à 50 FPS
        {
            ghost.update(DT);

            if (i % level.reactionFrames == 0)
            {
                WorldPoint g = ghost.getPosition();
                WorldPoint p = trainer.getPosition();
                // vitesses vues par le robot (comme un humain les "voit")
                double vx = (p.first - lastX) / DT, vy = (p.second - lastY) / DT;
                double vtx = (g.first - lastGhost.first) / DT, vty = (g.second - lastGhost.second) / DT;

                double ex = g.first - p.first, ey = g.second - p.second;
                if (level.anticipates)
                {
                    // Il vise un peu DEVANT la cible (là où elle sera dans 0,5 s)
                    // et freine selon sa propre vitesse.
                    ex += vtx * 0.5 - vx * 0.3;
                    ey += vty * 0.5 - vy * 0.3;
                }

                for (int key : { Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right })
                    trainer.onKeyRelease(key);

                if (ex > 0.3)  trainer.onKeyPress(Qt::Key_Right);
                if (ex < -0.3) trainer.onKeyPress(Qt::Key_Left);
                if (ey > 0.3)  trainer.onKeyPress(Qt::Key_Up);
                if (ey < -0.3) trainer.onKeyPress(Qt::Key_Down);

                if (u(rng) < level.randomness)
                {
                    const int keys[4] = { Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right };
                    trainer.onKeyPress(keys[static_cast<int>(u(rng) * 4) % 4]);
                }
            }

            lastX = trainer.getPosition().first;
            lastY = trainer.getPosition().second;
            lastGhost = ghost.getPosition();
            trainer.update(DT);
        }
        return trainer.getSamples();
    }

    std::string fmt(double v, int precision = 2)
    {
        std::ostringstream o;
        o << std::fixed << std::setprecision(precision) << v;
        return o.str();
    }
}

IATestReport runIATests()
{
    IATestReport report;
    auto check = [&](bool ok, const std::string& text)
    {
        report.lines.push_back(std::string(ok ? "[OK]     " : "[ECHEC] ") + text);
        if (!ok) report.allOk = false;
        qDebug().noquote() << QString::fromStdString(report.lines.back());
    };

    std::mt19937 rng(42); // graine fixe : mêmes résultats à chaque lancement

    // ---------- Q1 : sauvegarde + relecture de la trajectoire ----------
    std::vector<WorldPoint> traj = makeTestTrajectory();
    bool saved = saveTrajectory("test_trajectoire.txt", traj);
    std::vector<WorldPoint> reloaded = loadTrajectory("test_trajectoire.txt");
    double maxErr = 0.0;
    if (reloaded.size() == traj.size())
        for (size_t i = 0; i < traj.size(); ++i)
            maxErr = std::max(maxErr, std::abs(traj[i].first - reloaded[i].first)
                                    + std::abs(traj[i].second - reloaded[i].second));
    check(saved && reloaded.size() == traj.size() && maxErr < 1e-3,
          "Q1 trajectoire : " + std::to_string(reloaded.size()) + "/" + std::to_string(traj.size())
          + " positions relues");

    ReplayEntity replay(reloaded, QPixmap());
    replay.update(DT);
    check(!reloaded.empty() && replay.getPosition() == reloaded[1],
          "Q1 rejeu : le fantome avance d'une position par pas de temps");
    report.trajectory = reloaded;

    // ---------- Q2 à Q7 pour 3 niveaux de robot ----------
    const BotLevel levels[3] = {
        { "Robot facile",    "test_entrainement_facile.csv",    Qt::green,   25, 0.30, false },
        { "Robot moyen",     "test_entrainement_moyen.csv",     Qt::blue,    8,  0.05, false },
        { "Robot difficile", "test_entrainement_difficile.csv", Qt::red,     8,  0.0,  true  },
    };

    std::vector<std::pair<std::string, double>> ranking; // (niveau, distance de son IA)

    for (const auto& level : levels)
    {
        // Q2-Q3 : entraînement + sauvegarde + relecture
        std::vector<TrainingSample> samples = botTraining(level, reloaded, rng);
        saveTrainingData(level.file, samples);
        std::vector<TrainingSample> back = loadTrainingData(level.file);
        // distance moyenne du robot lui-même pendant l'entraînement
        double botDist = 0.0;
        for (const auto& s : back)
            botDist += std::sqrt(s.x[0] * s.x[0] + s.x[1] * s.x[1]);
        if (!back.empty())
            botDist /= back.size();

        check(samples.size() == 6000 && back.size() == samples.size(),
              std::string("Q2-Q3 ") + level.name + " : " + std::to_string(back.size())
              + " points enregistres (le robot etait a " + fmt(botDist) + " m en moyenne)");

        // Q4 : coefficients des hyperplans
        std::vector<double> cx, cy;
        bool trained = trainFromFile(level.file, cx, cy);
        check(trained, std::string("Q4 ") + level.name + " : 2 x 7 coefficients calcules");
        if (!trained)
            continue;

        // Q5-Q6 : l'IA poursuit le fantôme pendant 60 s (sans affichage)
        ReplayEntity ghost(reloaded, QPixmap());
        AIEnemyEntity ai({ -5.0, -4.0 }, &ghost, cx, cy, level.color, level.name);
        for (int i = 0; i < 3000; ++i)
        {
            ghost.update(DT);
            ai.update(DT);
        }
        double dist = ai.averageDistance();
        check(std::isfinite(dist),
              std::string("Q5-Q6 IA du ") + level.name + " : distance moyenne " + fmt(dist)
              + " m, contact " + fmt(ai.contactRatio() * 100.0, 0) + " %");

        ranking.push_back({ level.name, dist });

        report.ais.push_back({ std::string("IA ") + level.name, level.color, cx, cy });
    }

    // Q7 : le robot qui joue mal doit donner l'IA la plus faible
    // (celle qui reste le plus loin). Entre "moyen" et "difficile",
    // l'écart est faible : on l'affiche seulement, sans en faire un test.
    bool facileIsWorst = ranking.size() == 3
        && ranking[0].second > ranking[1].second
        && ranking[0].second > ranking[2].second;
    check(facileIsWorst, "Q7 le robot facile donne bien l'IA la plus faible");
    if (ranking.size() == 3)
        report.lines.push_back("[INFO]   Q7 distances des IA : facile " + fmt(ranking[0].second)
                               + " m | moyen " + fmt(ranking[1].second)
                               + " m | difficile " + fmt(ranking[2].second) + " m");

    report.lines.push_back(report.allOk ? "=> Tous les tests sont passes."
                                        : "=> Au moins un test a echoue (voir la fenetre Sortie).");
    return report;
}
