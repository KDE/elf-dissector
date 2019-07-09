/*
    Copyright (C) 2015 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

private slots:
    void replot();

private:
    Gnuplotter m_plotter;
};

#endif // GNUPLOTWIDGET_H
