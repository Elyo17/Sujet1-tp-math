#include <GameWindow.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GameWindow w;
    w.resize(800, 600);
    w.setWindowTitle("R5.A.12");
    w.show();

    return a.exec();
}
