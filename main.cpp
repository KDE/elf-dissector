#include <QApplication>

#include <elf/elffile.h>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    ElfFile file(app.applicationFilePath());
    file.parse();
//     return app.exec();
    return 0;
}
