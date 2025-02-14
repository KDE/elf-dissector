/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef GNUPLOTWIDGET_H
#define GNUPLOTWIDGET_H

#include <plotter/gnuplotter.h>

#include <QLabel>

class GnuplotWidget : public QLabel
{
    Q_OBJECT
public:
    explicit GnuplotWidget(QWidget *parent = nullptr);
    ~GnuplotWidget();

    void setPlotter(Gnuplotter &&plotter);

    QSize minimumSizeHint() const override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
    void replot();

private:
    Gnuplotter m_plotter;
};

#endif // GNUPLOTWIDGET_H
