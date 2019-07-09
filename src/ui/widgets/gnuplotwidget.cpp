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

#include "gnuplotwidget.h"

#include <QDebug>
#include <QPixmap>
#include <QResizeEvent>

GnuplotWidget::GnuplotWidget(QWidget* parent):
    QLabel(parent)
{
    setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (!Gnuplotter::hasGnuplot())
        setText(tr("'gnuplot' not found, plotting not available."));
}

GnuplotWidget::~GnuplotWidget() = default;

void GnuplotWidget::setPlotter(Gnuplotter&& plotter)
{
    m_plotter = std::move(plotter);
    replot();
}

QSize GnuplotWidget::minimumSizeHint() const
{
    // we can be made smaller than the content image, by re-rendering
    return QWidget::minimumSizeHint();
}

void GnuplotWidget::resizeEvent(QResizeEvent* event)
{
    replot();
    QWidget::resizeEvent(event);
}

void GnuplotWidget::replot()
{
    m_plotter.setSize(size());
    qDebug() << size() << m_plotter.isValid();
    if (!m_plotter.isValid())
        return;

    m_plotter.plot();
    QPixmap p;
    p.load(m_plotter.imageFileName());
    setPixmap(p);
}
