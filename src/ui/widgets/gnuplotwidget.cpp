/*
    SPDX-FileCopyrightText: 2015 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
