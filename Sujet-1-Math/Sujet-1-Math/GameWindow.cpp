#include <GameWindow.h>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include "PlayerEntity.h"
#include "FixedEntity.h"
#include "PatrolEntity.h"
#include "Entity.h"
#include <QApplication>



GameWindow::GameWindow(QWidget* parent) :
    QMainWindow(parent)
{

    timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, &GameWindow::updateGame);
    timer->start(20); // 50 FPS

    QPixmap carSprite(":/Sujet1Math/voiture.png");

    //Joueur 1 donc accès au touche flèche
    KeyBindings player1Keys{ Qt::Key_Up, Qt::Key_Down, Qt::Key_Left, Qt::Key_Right };
    entities.push_back(std::make_unique<PlayerEntity>(
        WorldPoint{ -2.0, 0.0 }, player1Keys, carSprite));

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

}

GameWindow::~GameWindow()
{
}

void GameWindow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::white);

    
    // Ceci est la boucle polymorphe. Elle appele draw
    for (auto& e : entities)
    {
        ScreenPoint screenPos = toScreen(e->getPosition());
        e->draw(painter, screenPos.first, screenPos.second);
    }

    //C'est un texte de debug
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    int y = 30;
    for (auto& e : entities)
    {
        PlayerEntity* player = dynamic_cast<PlayerEntity*>(e.get());
        if (player != nullptr)
        {
            std::string txt = "vitesse x : " + std::to_string(player->getVx()) +
                " m/s | vitesse y : " + std::to_string(player->getVy()) + " m/s";
            painter.drawText(20, y, QString::fromStdString(txt));
            y += 25;
        }
    }
}
void GameWindow::updateGame()
{

    const double dt = 0.02; // 20 ms, cohérent avec timer->start(20)

 
   // Les limites de rebond dépendent de la taille de LA FENETRE
   // (width()/height()), donc SEULE GameWindow peut les calculer
   // (via toPhysical, qui utilise _zoom). On les calcule une fois
   // ici, puis on les distribue à chaque PlayerEntity trouvé dans
   // la liste (les autres entités n'en ont pas besoin).
    WorldPoint minPoint = toPhysical({ 50, height() - 30 });
    WorldPoint maxPoint = toPhysical({ width() - 50, 30 });

    PlayerEntity* player = nullptr;
    for (auto& e : entities)
    {
        // On donne les limites SEULEMENT si l'entité est bien un
        // PlayerEntity (setBounds n'existe pas dans l'interface
        // Entity, donc on ne peut l'appeler qu'après un
        // dynamic_cast réussi).
        PlayerEntity* p = dynamic_cast<PlayerEntity*>(e.get());
        if (p != nullptr)
        {
            player = p;
            player->setBounds(minPoint, maxPoint);
          
        }

        // En revanche, update() EST dans l'interface Entity : on
        // peut l'appeler directement sur tout le monde, sans
        // dynamic_cast, sans savoir de quel type est réellement
        // l'entité. C'est la boucle "générique" du moteur de jeu.
        e->update(dt);
    }


    // boucle pour les collisions de chaque joueur
    for (auto& e : entities)
    {
        PlayerEntity* player = dynamic_cast<PlayerEntity*>(e.get());

        //si c'est un joueur
        if (player != nullptr) 
        {
            bool hascollided = false;

            for (auto it = entities.begin(); it != entities.end(); ++it)
            {
                auto& ent = *it;
                if (ent.get() != player && player->isColliding(*ent))
                {
                    hascollided = true; // pour tester affichage

                    if (FixedEntity* resource = dynamic_cast<FixedEntity*>(ent.get())) // en cas de ressource, suprimme la ressource
                    {
                        entities.erase(it);
                        break;
                    }
                    else if (PatrolEntity* enemy = dynamic_cast<PatrolEntity*>(ent.get())) // en cas d'ennemis, quitte le jeu
                    {
                        QApplication::quit();
                    }
                }
            }

            player->ChangeCollider(hascollided);
        }
    }

    update(); // redemande un paintEvent (méthode héritée de QWidget)
}

void GameWindow::keyPressEvent(QKeyEvent* event)
{
    
    // GameWindow transmet la touche pressée à TOUTES les entités.
    // Chacune décide elle-même si ça la concerne :
    //   - PlayerEntity compare "key" à ses propres KeyBindings
    //   - FixedEntity / PatrolEntity ignorent l'appel (comportement
    //     vide hérité d'Entity, jamais redéfini)
    // GameWindow n'a donc PLUS BESOIN de savoir combien de joueurs
    // il y a, ni quelles touches ils utilisent : tout est délégué.
    for (auto& e : entities)
    {
        e->onKeyPress(event->key());
    }
    QWidget::keyPressEvent(event);
}


void GameWindow::keyReleaseEvent(QKeyEvent* event)
{
    for (auto& e : entities)
    {
        e->onKeyRelease(event->key());
    }
    QWidget::keyReleaseEvent(event);
}

ScreenPoint GameWindow::toScreen(const WorldPoint& point) const
{
    double x = point.first;
    double y = point.second;
    int X = static_cast<int>(width() / 2.0 + _zoom * x);
    int Y = static_cast<int>(height() / 2.0 - _zoom * y);
    return { X, Y };
}

WorldPoint GameWindow::toPhysical(const ScreenPoint& point) const
{
    double X = point.first;
    double Y = point.second;
    double x = (X - width() / 2.0) / _zoom;
    double y = (height() / 2.0 - Y) / _zoom;
    return { x, y };
}
