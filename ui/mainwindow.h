#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <memory>

class TreeMapWidget;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void fileOpen();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    TreeMapWidget *m_treeMap = 0;
};

#endif // MAINWINDOW_H
