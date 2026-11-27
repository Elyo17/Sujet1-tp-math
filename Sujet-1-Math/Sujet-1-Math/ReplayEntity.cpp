#include "ReplayEntity.h"
#include <QPainter>
#include <fstream>
#include <cmath>

bool saveTrajectory(const std::string& filename, const std::vector<WorldPoint>& points)
{
    std::ofstream file(filename);
    if (!file)
        return false;

    // Une ligne par pas de temps : "x y"
    for (const auto& p : points)
        file << p.first << ' ' << p.second << '\n';
    return true;
}

std::vector<WorldPoint> loadTrajectory(const std::string& filename)
{
    std::vector<WorldPoint> points;
    std::ifstream file(filename);
    double x, y;
    while (file >> x >> y)
        points.push_back({ x, y });
    return points;
}

ReplayEntity::ReplayEntity(const std::vector<WorldPoint>& traj, QPixmap sprite)
    : Entity(traj.empty() ? WorldPoint{ 0.0, 0.0 } : traj.front(), 0.7)
    , trajectory(traj)
    , sprite(sprite)
{
}

void ReplayEntity::update(double dt)
{
    (void)dt;
    if (trajectory.empty())
        return;

    // On avance d'une case : 1 position enregistrée = 1 pas de temps.
    // Le rejeu est donc à la même vitesse que l'enregistrement.
    size_t next = (index + 1) % trajectory.size();

    double dx = trajectory[next].first - trajectory[index].first;
    double dy = trajectory[next].second - trajectory[index].second;
    if (std::abs(dx) + std::abs(dy) > 1e-9)
        angle = std::atan2(dy, dx) * 180.0 / M_PI;

    index = next;
    position = trajectory[index];
}

void ReplayEntity::draw(QPainter& painter, int screenX, int screenY)
{
    painter.save();
    painter.translate(screenX, screenY);
    painter.rotate(-angle);
    painter.setOpacity(0.6); // un peu transparent : c'est un "fantôme"
    painter.drawPixmap(-50, -30, 100, 60, sprite);
    painter.restore();
}
