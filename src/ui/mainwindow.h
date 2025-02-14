/*
    SPDX-FileCopyrightText: 2013-2014 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <memory>

class ElfFileSet;
class ElfModel;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    void loadFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
    void fileOpen();
    void reloadFileOnStartup();
    void currentViewChanged();

private:
    void restoreSettings();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    QString m_currentFileName;
    ElfModel *m_elfModel = nullptr;
    std::unique_ptr<ElfFileSet> m_fileSet;
};

#endif // MAINWINDOW_H
