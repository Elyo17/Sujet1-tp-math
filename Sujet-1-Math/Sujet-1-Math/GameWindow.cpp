#include "GameWindow.h"
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include <algorithm>
#include "PlayerEntity.h"
#include "FixedEntity.h"
#include "PatrolEntity.h"
#include "EnemyEntity.h"
#include "FollowerEntity.h"
#include "ReplayEntity.h"
#include "TrainingEntity.h"
#include "AIEnemyEntity.h"
#include "IAConfig.h"
#include "IATests.h"

// ************************************************************
//   CHOIX DU MODE : c'est la seule ligne à changer entre 2 lancements
//   Ordre normal : Enregistrement -> Entrainement -> IA
//   GameMode::Test lance les tests automatiques (rien à jouer)
// ************************************************************
constexpr GameMode MODE = GameMode::IA;

// Mode IA (questions 6-7) : les fichiers d'entraînement à charger.
// Un ennemi IA est créé par fichier trouvé. Pour la question 7, renomme
// tes fichiers après chaque entraînement (ex : entrainement_facile.csv)
// et ajoute-les ici : tous les ennemis courront en même temps.
struct AIProfile { const char* file; QColor color; const char* name; };
const std::vector<AIProfile> AI_PROFILES = {
    { "entrainement.csv",           Qt::magenta,   "IA (entrainement.csv)" },
    { "entrainement_facile.csv",    Qt::green,     "IA facile" },
    { "entrainement_moyen.csv",     Qt::blue,      "IA moyen" },
    { "entrainement_difficile.csv", Qt::red,       "IA difficile" },
};


GameWindow::GameWindow(QWidget* parent) :
    QMainWindow(parent)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameWindow::updateGame);
    timer->start(20); // 50 FPS

    switch (MODE)
    {
    case GameMode::Jeu:            setupJeu(); break;
    case GameMode::Enregistrement: setupEnregistrement(); break;
    case GameMode::Entrainement:   setupEntrainement(); break;
    case GameMode::IA:             setupIA(); break;
    case GameMode::Test:           setupTest(); break;
    }
}

// ------------------------------------------------------------
// Le jeu des sections précédentes (ton ancien constructeur, inchangé).
// ------------------------------------------------------------
void GameWindow::setupJeu()
{
    // === Contenu identique à ton ancien constructeur ===
    QPixmap carSprite(":/Sujet1Math/voiture.png");

    //Joueur 1 donc accès au touche flèche
    KeyBindings player1Keys{ Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right };
    entities.push_back(std::make_unique<PlayerEntity>(
        WorldPoint{ -2.0, 0.0 }, player1Keys, carSprite));

    Entity* joueur1 = entities[0].get();

    EnemyProfile intermediaire{ 1208.0, 1.8, 40.0, 300.0, 100.0, ChaseMode::ProportionnelAmorti };
    entities.push_back(std::make_unique<EnemyEntity>(
        WorldPoint{ 5.0, 4.0 }, joueur1, intermediaire, Qt::magenta));

    // Joueur 2 avec les touches ZQSD
    KeyBindings player2Keys{ Qt::Key_Z, Qt::Key_S, Qt::Key_Q, Qt::Key_D };
    entities.push_back(std::make_unique<PlayerEntity>(
        WorldPoint{ 2.0, 0.0 }, player2Keys, carSprite));

    //Une entité fixe par exemple un mur au centre du terrain
    entities.push_back(std::make_unique<FixedEntity>(
        WorldPoint{ 0.0, 2.0 }, 40.0, 40.0, Qt::darkGray));

    //L'entité Patrouille qui fait des allers-retours
    entities.push_back(std::make_unique<PatrolEntity>(
        WorldPoint{ -3.0, -3.0 }, WorldPoint{ 3.0, -3.0 },
        1.5 /* vitesse */, 30.0, 30.0, Qt::red));

    auto suiveur = std::make_unique<FollowerEntity>(WorldPoint{ 6.0, 5.0 }, joueur1, Qt::cyan);
    suiveur->setTangentialCoef(0.0);   // question 3 : 2nd terme désactivé
    entities.push_back(std::move(suiveur));
}

// ------------------------------------------------------------
// Q1 (partie 1) : tu joues seul, ta trajectoire est enregistrée.
// Elle est sauvegardée dans trajectoire.txt à la fermeture.
// ------------------------------------------------------------
void GameWindow::setupEnregistrement()
{
    QPixmap carSprite(":/Sujet1Math/voiture.png");
    KeyBindings keys{ Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right };
    // Surtout PAS en (0, 0) : c'est le centre de la force centrale de
    // PlayerEntity, et forceCentraleX() y divise par 0 (NaN).
    auto player = std::make_unique<PlayerEntity>(WorldPoint{ -2.0, 0.0 }, keys, carSprite);
    recordedPlayer = player.get();
    entities.push_back(std::move(player));
}

// ------------------------------------------------------------
// Q2-Q3 : le fantôme rejoue trajectoire.txt, tu pilotes le carré
// jaune (flèches) pour le suivre et le toucher. Tout est sauvegardé
// dans entrainement.csv à la fermeture.
// ------------------------------------------------------------
void GameWindow::setupEntrainement()
{
    QPixmap carSprite(":/Sujet1Math/voiture.png");
    auto traj = loadTrajectory(IA::TRAJECTORY_FILE);
    if (traj.empty())
        qWarning() << "trajectoire.txt introuvable : lance d'abord le mode Enregistrement !";

    auto ghost = std::make_unique<ReplayEntity>(traj, carSprite);
    Entity* ghostPtr = ghost.get();
    entities.push_back(std::move(ghost));

    auto trainer = std::make_unique<TrainingEntity>(WorldPoint{ 5.0, 4.0 }, ghostPtr, Qt::yellow);
    trainee = trainer.get();
    entities.push_back(std::move(trainer));
}

// ------------------------------------------------------------
// Q4-Q7 : calcul des coefficients puis course des ennemis IA.
// ------------------------------------------------------------
void GameWindow::setupIA()
{
    QPixmap carSprite(":/Sujet1Math/voiture.png");
    auto traj = loadTrajectory(IA::TRAJECTORY_FILE);
    if (traj.empty())
        qWarning() << "trajectoire.txt introuvable : lance d'abord le mode Enregistrement !";

    auto ghost = std::make_unique<ReplayEntity>(traj, carSprite);
    Entity* ghostPtr = ghost.get();
    entities.push_back(std::move(ghost));

    for (const auto& profile : AI_PROFILES)
    {
        std::vector<double> cx, cy;
        if (trainFromFile(profile.file, cx, cy)) // Q4
        {
            // Q5 : tous partent du même point que l'entité d'entraînement
            entities.push_back(std::make_unique<AIEnemyEntity>(
                WorldPoint{ 5.0, 4.0 }, ghostPtr, cx, cy, profile.color, profile.name));
        }
    }
}

// ------------------------------------------------------------
// Mode Test : un robot fait les entraînements, on vérifie chaque
// question, puis les 3 IA obtenues poursuivent le fantôme à l'écran.
// ------------------------------------------------------------
void GameWindow::setupTest()
{
    QPixmap carSprite(":/Sujet1Math/voiture.png");
    IATestReport report = runIATests();
    testLines = report.lines;

    auto ghost = std::make_unique<ReplayEntity>(report.trajectory, carSprite);
    Entity* ghostPtr = ghost.get();
    entities.push_back(std::move(ghost));

    for (const auto& ai : report.ais)
        entities.push_back(std::make_unique<AIEnemyEntity>(
            WorldPoint{ -5.0, -4.0 }, ghostPtr, ai.coefX, ai.coefY, ai.color, ai.name));
}

GameWindow::~GameWindow()
{
    // Sauvegardes automatiques à la fermeture de la fenêtre
    if (MODE == GameMode::Enregistrement)
    {
        if (saveTrajectory(IA::TRAJECTORY_FILE, recordedTrajectory))
            qDebug() << "Trajectoire sauvegardee :" << recordedTrajectory.size() << "positions";
    }
    if (MODE == GameMode::Entrainement && trainee != nullptr)
    {
        if (saveTrainingData(IA::TRAINING_FILE, trainee->getSamples()))
            qDebug() << "Donnees d'entrainement sauvegardees :" << trainee->getSamples().size() << "points";
    }
}

void GameWindow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    for (auto& e : entities)
    {
        ScreenPoint screenPos = toScreen(e->getPosition());
        e->draw(painter, screenPos.first, screenPos.second);
    }

    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    int y = 25;
    auto line = [&](const QString& txt) { painter.drawText(15, y, txt); y += 22; };

    switch (MODE)
    {
    case GameMode::Jeu:
    {
        // === Ton texte de debug d'origine ===
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        int yJeu = 30;
        for (auto& e : entities)
        {
            PlayerEntity* player = dynamic_cast<PlayerEntity*>(e.get());
            if (player != nullptr)
            {
                std::string txt = "vitesse x : " + std::to_string(player->getVx()) +
                    " m/s | vitesse y : " + std::to_string(player->getVy()) + " m/s" + " | score : " + std::to_string(player->getScore());
                painter.drawText(20, yJeu, QString::fromStdString(txt));
                yJeu += 25;
            }
        }
        break;
    }

    case GameMode::Enregistrement:
        line(QString("ENREGISTREMENT : %1 positions (%2 s) - ferme la fenetre pour sauvegarder")
             .arg(recordedTrajectory.size()).arg(recordedTrajectory.size() * 0.02, 0, 'f', 1));
        break;

    case GameMode::Entrainement:
        if (trainee)
            line(QString("ENTRAINEMENT : %1 points (%2 s) - poursuis le fantome avec les fleches")
                 .arg(trainee->getSamples().size())
                 .arg(trainee->getSamples().size() * 0.02, 0, 'f', 1));
        break;

    case GameMode::IA:
        for (auto& e : entities)
            if (auto* ai = dynamic_cast<AIEnemyEntity*>(e.get()))
                line(QString("%1 : distance moyenne %2 m | en contact %3 % du temps")
                     .arg(ai->getName().c_str())
                     .arg(ai->averageDistance(), 0, 'f', 2)
                     .arg(ai->contactRatio() * 100.0, 0, 'f', 0));
        break;

    case GameMode::Test:
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        for (const auto& t : testLines)
        {
            painter.setPen(t.rfind("[ECHEC]", 0) == 0 ? Qt::red : Qt::black);
            line(QString::fromStdString(t));
        }
        painter.setPen(Qt::black);
        line("Mesures en direct depuis le lancement :");
        for (auto& e : entities)
            if (auto* ai = dynamic_cast<AIEnemyEntity*>(e.get()))
                line(QString("  %1 : distance moyenne %2 m | contact %3 %")
                     .arg(ai->getName().c_str())
                     .arg(ai->averageDistance(), 0, 'f', 2)
                     .arg(ai->contactRatio() * 100.0, 0, 'f', 0));
        break;
    }
}

void GameWindow::updateGame()
{
    const double dt = 0.02;

    WorldPoint minPoint = toPhysical({ 50, height() - 30 });
    WorldPoint maxPoint = toPhysical({ width() - 50, 30 });

    // === Ta boucle d'origine : chaque EnemyEntity vise le joueur le plus proche ===
    for (auto& e : entities)
    {
        EnemyEntity* enemy = dynamic_cast<EnemyEntity*>(e.get());
        if (enemy == nullptr) continue;

        enemy->setBounds(minPoint, maxPoint);   // ne sort plus de l'écran

        // Cherche le joueur le plus proche
        Entity* plusProche = nullptr;
        double meilleureDist = 1e18;
        for (auto& other : entities)
        {
            if (dynamic_cast<PlayerEntity*>(other.get()) == nullptr) continue;
            double dx = other->getPosition().first - enemy->getPosition().first;
            double dy = other->getPosition().second - enemy->getPosition().second;

            double d = dx * dx + dy * dy;
            if (d < meilleureDist) { meilleureDist = d; plusProche = other.get(); }
        }
        enemy->setTarget(plusProche);
    }

    for (auto& e : entities)
    {
        if (auto* p = dynamic_cast<PlayerEntity*>(e.get()))
            p->setBounds(minPoint, maxPoint);
        e->update(dt);
    }

    // Q1 : on note la position du joueur à chaque pas de temps
    if (MODE == GameMode::Enregistrement && recordedPlayer != nullptr)
        recordedTrajectory.push_back(recordedPlayer->getPosition());

    if (MODE == GameMode::Jeu)
        handleCollisions();

    update();
}

// Collisions du mode Jeu. Correction du bug signalé plus tôt : on ne
// supprime plus pendant qu'on parcourt la liste, on note d'abord les
// entités à supprimer, puis on les supprime après les boucles.
void GameWindow::handleCollisions()
{
    std::vector<Entity*> toRemove;

    for (auto& e : entities)
    {
        auto* player = dynamic_cast<PlayerEntity*>(e.get());
        if (player == nullptr)
            continue;

        for (auto& other : entities)
        {
            Entity* ent = other.get();
            if (ent == player || !player->isColliding(*ent))
                continue;

            if (dynamic_cast<FixedEntity*>(ent))
            {
                if (std::find(toRemove.begin(), toRemove.end(), ent) == toRemove.end())
                {
                    player->addScore();
                    toRemove.push_back(ent);
                }
            }
            else if (dynamic_cast<PatrolEntity*>(ent) || dynamic_cast<EnemyEntity*>(ent))
            {
                QApplication::quit(); // game over
                return;
            }
        }
    }

    entities.erase(std::remove_if(entities.begin(), entities.end(),
        [&](const std::unique_ptr<Entity>& e) {
            return std::find(toRemove.begin(), toRemove.end(), e.get()) != toRemove.end();
        }), entities.end());
}

void GameWindow::keyPressEvent(QKeyEvent* event)
{
    for (auto& e : entities)
        e->onKeyPress(event->key());
    QWidget::keyPressEvent(event);
}

void GameWindow::keyReleaseEvent(QKeyEvent* event)
{
    for (auto& e : entities)
        e->onKeyRelease(event->key());
    QWidget::keyReleaseEvent(event);
}

ScreenPoint GameWindow::toScreen(const WorldPoint& point) const
{
    int X = static_cast<int>(width() / 2.0 + _zoom * point.first);
    int Y = static_cast<int>(height() / 2.0 - _zoom * point.second);
    return { X, Y };
}

WorldPoint GameWindow::toPhysical(const ScreenPoint& point) const
{
    double x = (point.first - width() / 2.0) / _zoom;
    double y = (height() / 2.0 - point.second) / _zoom;
    return { x, y };
}
