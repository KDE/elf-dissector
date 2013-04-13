#include <ui/mainwindow.h>
#include <QApplication>

int main(int argc, char** argv)
{
    QCoreApplication::setApplicationName("ELF Dissector");
    QCoreApplication::setOrganizationName("KDAB");
    QCoreApplication::setOrganizationDomain("kdab.com");

    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
