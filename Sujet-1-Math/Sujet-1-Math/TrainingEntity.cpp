#include "TrainingEntity.h"
#include <QPainter>
#include <fstream>
#include <sstream>

void TrainingEntity::onKeyPress(int key)
{
    if (key == Qt::Key_Up)    up = true;
    if (key == Qt::Key_Down)  down = true;
    if (key == Qt::Key_Left)  left = true;
    if (key == Qt::Key_Right) right = true;
}

void TrainingEntity::onKeyRelease(int key)
{
    if (key == Qt::Key_Up)    up = false;
    if (key == Qt::Key_Down)  down = false;
    if (key == Qt::Key_Left)  left = false;
    if (key == Qt::Key_Right) right = false;
}

void TrainingEntity::update(double dt)
{
    if (target == nullptr)
        return;

    // --- 1. La situation actuelle (les 6 variables d'entrée) ---
    WorldPoint tp = target->getPosition();
    double vtx, vty;
    tracker.measure(tp, dt, vtx, vty);
    double dx = tp.first - position.first;
    double dy = tp.second - position.second;

    // --- 2. Ta décision : les propulsions venant du clavier ---
    double px = 0.0, py = 0.0;
    if (right) px += IA::MAX_THRUST;
    if (left)  px -= IA::MAX_THRUST;
    if (up)    py += IA::MAX_THRUST;
    if (down)  py -= IA::MAX_THRUST;

    // --- 3. Question (3) : on enregistre (situation -> décision) ---
    samples.push_back({ { dx, dy, vtx, vty, Vx, Vy }, px, py });

    // --- 4. On applique la physique ---
    IA::step(position, Vx, Vy, px, py, dt);
}

void TrainingEntity::draw(QPainter& painter, int screenX, int screenY)
{
    painter.save();
    painter.setBrush(color);
    painter.setPen(Qt::black);
    painter.drawRect(screenX - 10, screenY - 10, 20, 20);
    painter.restore();
}

// Format CSV (ouvrable dans Excel / LibreOffice) :
// dx;dy;vtx;vty;vx;vy;px;py
bool saveTrainingData(const std::string& filename, const std::vector<TrainingSample>& samples)
{
    std::ofstream file(filename);
    if (!file)
        return false;

    file << "dx;dy;vtx;vty;vx;vy;px;py\n";
    for (const auto& s : samples)
    {
        for (double v : s.x)
            file << v << ';';
        file << s.px << ';' << s.py << '\n';
    }
    return true;
}

std::vector<TrainingSample> loadTrainingData(const std::string& filename)
{
    std::vector<TrainingSample> samples;
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // saute la ligne d'en-tête

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cell;
        std::vector<double> values;
        while (std::getline(ss, cell, ';'))
        {
            // istringstream plutôt que std::stod : stod dépend de la langue
            // du système (en français il attendrait "0,5" au lieu de "0.5").
            std::istringstream cellStream(cell);
            double v;
            if (cellStream >> v)
                values.push_back(v);
        }

        if (values.size() != 8)
            continue; // ligne incomplète : on l'ignore

        TrainingSample s;
        for (int i = 0; i < 6; ++i)
            s.x[i] = values[i];
        s.px = values[6];
        s.py = values[7];
        samples.push_back(s);
    }
    return samples;
}
