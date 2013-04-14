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
    void hideDebugInformation();
    void reloadFileOnStartup();
    void colorizationChanged();
    void treeMapContextMenu(const QPoint &pos);

private:
    void restoreSettings();
    void loadFile(const QString &fileName);

private:
    std::unique_ptr<Ui::MainWindow> ui;
    TreeMapWidget *m_treeMap = 0;
    QString m_currentFileName;
};

#endif // MAINWINDOW_H
