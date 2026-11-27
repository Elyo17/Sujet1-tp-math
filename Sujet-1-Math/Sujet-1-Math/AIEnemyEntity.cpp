#include "AIEnemyEntity.h"
#include "TrainingEntity.h" // pour loadTrainingData
#include "IATrainData.h"    // pour computeHyperplanCoefficients
#include <QPainter>
#include <QDebug>
#include <algorithm>
#include <cmath>

bool trainFromFile(const std::string& filename,
                   std::vector<double>& coefX, std::vector<double>& coefY)
{
    std::vector<TrainingSample> samples = loadTrainingData(filename);

    // On fabrique 2 jeux de données avec les MEMES entrées,
    // mais une sortie différente : px pour l'un, py pour l'autre.
    std::vector<IATrainData> dataX, dataY;
    for (const auto& s : samples)
    {
        std::vector<double> x(s.x.begin(), s.x.end());
        dataX.push_back({ x, s.px });
        dataY.push_back({ x, s.py });
    }

    coefX = computeHyperplanCoefficients(dataX);
    coefY = computeHyperplanCoefficients(dataY);

    if (coefX.size() != 7 || coefY.size() != 7)
    {
        qWarning() << "Entrainement impossible avec" << filename.c_str()
                   << ":" << samples.size() << "points (fichier absent ou trop court)";
        return false;
    }

    // Affiche les coefficients dans la fenêtre "Sortie" de Visual Studio
    qDebug() << "=== Coefficients appris depuis" << filename.c_str()
             << "(" << samples.size() << "points ) ===";
    qDebug() << "        c0      dx      dy     vtx     vty      vx      vy";
    QString lx = "px :", ly = "py :";
    for (int i = 0; i < 7; ++i)
    {
        lx += QString(" %1").arg(coefX[i], 7, 'f', 2);
        ly += QString(" %1").arg(coefY[i], 7, 'f', 2);
    }
    qDebug().noquote() << lx;
    qDebug().noquote() << ly;
    return true;
}

double AIEnemyEntity::evaluate(const std::vector<double>& c, const IA::Features& x)
{
    if (c.size() != 7)
        return 0.0;

    // L'équation de l'hyperplan
    double p = c[0];
    for (int i = 0; i < 6; ++i)
        p += c[i + 1] * x[i];
    return p;
}

void AIEnemyEntity::update(double dt)
{
    if (target == nullptr)
        return;

    // --- 1. La situation, calculée EXACTEMENT comme à l'entraînement ---
    WorldPoint tp = target->getPosition();
    double vtx, vty;
    tracker.measure(tp, dt, vtx, vty);
    double dx = tp.first - position.first;
    double dy = tp.second - position.second;
    IA::Features x = { dx, dy, vtx, vty, Vx, Vy };

    // --- 2. La décision, donnée par les hyperplans ---
    double px = evaluate(coefX, x);
    double py = evaluate(coefY, x);

    // Pendant l'entraînement, tu ne pouvais pas pousser plus fort que
    // MAX_THRUST. On impose la même limite à l'IA, pour qu'elle ne
    // soit pas plus puissante que toi (comparaison équitable).
    px = std::clamp(px, -IA::MAX_THRUST, IA::MAX_THRUST);
    py = std::clamp(py, -IA::MAX_THRUST, IA::MAX_THRUST);

    // --- 3. Même physique que l'entité d'entraînement ---
    IA::step(position, Vx, Vy, px, py, dt);

    // --- 4. Statistiques pour la question (6) ---
    double dist = std::sqrt(dx * dx + dy * dy);
    sumDistance += dist;
    frames++;
    if (isColliding(*target))
        contactFrames++;
}

void AIEnemyEntity::draw(QPainter& painter, int screenX, int screenY)
{
    painter.save();
    painter.setBrush(color);
    painter.setPen(Qt::black);
    // Un losange, pour le distinguer des autres entités
    QPoint pts[4] = { {screenX, screenY - 12}, {screenX + 12, screenY},
                      {screenX, screenY + 12}, {screenX - 12, screenY} };
    painter.drawPolygon(pts, 4);
    painter.restore();
}
