#include "IDE/amainwindow.h"
#include <QApplication>
#include "src/application.h"

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    //Deux ligne pour initialiser les langues et charger une langue par défaut
    AMainWindow w;
    w.show();
    //Slot pour quitter le programme
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));

    return a.exec();
}
