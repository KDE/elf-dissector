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

#include "sizetreemapview.h"
#include "ui_sizetreemapview.h"

#include <treemap/treemap.h>
#include <colorizer.h>
#include <elfmodel/elfmodel.h>
#include <elfmodel/sectionproxymodel.h>

#include <elf/elffile.h>
#include <elf/elfsymboltablesection.h>
#include <demangle/demangler.h>

#include <QMenu>
#include <QSettings>

#include <elf.h>

SizeTreeMapView::SizeTreeMapView(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::SizeTreeMapView),
    m_sectionProxy(new SectionProxyModel(this))
{
    ui->setupUi(this);

    ui->sectionView->setModel(m_sectionProxy);
    m_sectionProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    connect(ui->searchLineEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        m_sectionProxy->setFilterFixedString(text);
    });

    ui->actionHideDebugInformation->setData("HideDebugInfo");
    ui->actionHideOccupiesMemory->setData("HideOccupiesNoMemory");
    ui->actionHideNonWritable->setData("HideNonWritable");
    ui->actionHideNonExecutable->setData("HideNonExecutable");

    ui->actionNoColorization->setData("NoColorize");
    ui->actionColorizeSections->setData("ColorizeSections");
    ui->actionColorizeSymbols->setData("ColorizeSymbols");
    ui->actionRelocationHeatmap->setData("RelocationHeatmap");

    auto colorizeGroup = new QActionGroup(this);
    colorizeGroup->setExclusive(true);
    colorizeGroup->addAction(ui->actionNoColorization);
    colorizeGroup->addAction(ui->actionColorizeSections);
    colorizeGroup->addAction(ui->actionColorizeSymbols);
    colorizeGroup->addAction(ui->actionRelocationHeatmap);

    auto separator = new QAction(this);
    separator->setSeparator(true);
    addActions({
        ui->actionHideDebugInformation,
        ui->actionHideOccupiesMemory,
        ui->actionHideNonWritable,
        ui->actionHideNonExecutable,
        separator,
        ui->actionNoColorization,
        ui->actionColorizeSections,
        ui->actionColorizeSymbols,
        ui->actionRelocationHeatmap
    });

    foreach (auto action, actions())
        connect(action, &QAction::toggled, this, &SizeTreeMapView::viewActionToggled);

    restoreSettings();
}

SizeTreeMapView::~SizeTreeMapView() = default;

static double relocRatio(ElfSymbolTableEntry *symbol)
{
    if (symbol->size() <= 0)
        return 0;
    return (double)(symbol->symbolTable()->file()->reverseRelocator()->relocationCount(symbol->value(), symbol->size()) * symbol->symbolTable()->file()->addressSize()) / symbol->size();
}

static double relocRatio(ElfSectionHeader *shdr)
{
    if (shdr->size() <= 0)
        return 0;
    return (double)(shdr->file()->reverseRelocator()->relocationCount(shdr->virtualAddress(), shdr->size()) * shdr->file()->addressSize()) / shdr->size();
}

static QColor relocColor(double ratio)
{
    return QColor(
        std::min(255.0, ratio * 510),
        std::min(255.0, (1-ratio) * 510),
        0
    );
}

void SizeTreeMapView::setModel(QAbstractItemModel* model)
{
    m_sectionProxy->setSourceModel(model);
    ui->sectionView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    connect(ui->sectionView->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this]() {
        m_viewDirty = true;
        reloadTreeMap();
    });
}

void SizeTreeMapView::reloadTreeMap()
{
    if (!m_viewDirty)
        return;
    m_viewDirty = false;

    const auto selection = ui->sectionView->selectionModel()->selectedRows();
    if (selection.isEmpty())
        return;

    const auto idx = selection.first();
    auto file = idx.data(ElfModel::FileRole).value<ElfFile*>();
    const auto section = idx.data(ElfModel::SectionRole).value<ElfSection*>();
    if (!file && section)
        file = section->file();
    if (!file)
        return;

    // TODO inefficient and leacky...
    auto baseItem = new TreeMapItem;
    baseItem->setText(0, file->displayName());
    baseItem->setSum(file->size());

    QList<int> prevSplitterSize;
    if (m_treeMap)
        prevSplitterSize = ui->splitter->sizes();

    delete m_treeMap;
    m_treeMap = new TreeMapWidget(baseItem, this);

    m_treeMap->setBorderWidth(3);
//     m_treeMap->setMinimalArea(200);
//     m_treeMap->setVisibleWidth(10, false);
    // looks weird, but this forces m_treeMap->_attrs to be resided correctly for text to be drawn
    m_treeMap->setFieldForced(1, true);
    m_treeMap->setFieldForced(1, false);
    m_treeMap->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_treeMap, &TreeMapWidget::customContextMenuRequested, this, &SizeTreeMapView::treeMapContextMenu);

    ui->splitter->addWidget(m_treeMap);
    if (!prevSplitterSize.isEmpty())
        ui->splitter->setSizes(prevSplitterSize);
    else
        ui->splitter->setSizes({ ui->sectionView->header()->sectionSize(0), width() - ui->sectionView->header()->sectionSize(0) });

    QSettings settings;
    m_treeMap->setSplitMode(settings.value(QStringLiteral("TreeMap/SplitMode"), "Bisection").toString());

    struct SymbolNode {
        TreeMapItem *item;
        QHash<QByteArray, SymbolNode*> children;
    };

    QVector<SymbolNode*> sectionItems;
    sectionItems.resize(file->sectionHeaders().size());

    if (!section) {
        Colorizer sectionColorizer(96, 255);
        foreach (const auto shdr, file->sectionHeaders()) {
            if (isSectionHidden(shdr)) {
                baseItem->setSum(baseItem->sum() - shdr->size());
                continue;
            }
            auto item = new TreeMapItem(baseItem, shdr->size(), shdr->name(), QString::number(shdr->size()));
            item->setSum(shdr->size());
            if (ui->actionColorizeSections->isChecked())
                item->setBackColor(sectionColorizer.nextColor());
            if (ui->actionRelocationHeatmap->isChecked() && shdr->flags() & SHF_WRITE)
                item->setBackColor(relocColor(relocRatio(shdr)));
            sectionItems[shdr->sectionIndex()] = new SymbolNode;
            sectionItems[shdr->sectionIndex()]->item = item;
        }
    } else {
        baseItem->setSum(section->header()->size());
        auto item = new TreeMapItem(baseItem, section->header()->size(), section->header()->name(), QString::number(section->header()->size()));
        item->setSum(section->header()->size());
        if (ui->actionRelocationHeatmap->isChecked() && section->header()->flags() & SHF_WRITE)
            item->setBackColor(relocColor(relocRatio(section->header())));
        sectionItems[section->header()->sectionIndex()] = new SymbolNode;
        sectionItems[section->header()->sectionIndex()]->item = item;
    }

    Colorizer symbolColorizer;
    Demangler demangler;

    const auto symtab = file->symbolTable();
    if (symtab) {
        for (unsigned int j = 0; j < symtab->header()->entryCount(); ++j) {
            const auto entry = symtab->entry(j);
            if (entry->size() == 0 || !sectionItems.at(entry->sectionIndex()))
                continue;
            SymbolNode *parentNode = sectionItems.at(entry->sectionIndex());
            const QVector<QByteArray> demangledNames = demangler.demangle(entry->name());
            for (const QByteArray &demangledName : demangledNames) {
                SymbolNode *node = parentNode->children.value(demangledName);
                if (!node) {
                    node = new SymbolNode;
                    node->item = new TreeMapItem(parentNode->item);
                    node->item->setField(0, demangledName);
                    if (ui->actionColorizeSymbols->isChecked() && parentNode->item->parent() == baseItem) {
                        node->item->setBackColor(symbolColorizer.nextColor());
                    } else {
                        node->item->setBackColor(parentNode->item->backColor());
                    }
                    parentNode->children.insert(demangledName, node);
                }
                node->item->setSum(node->item->sum() + entry->size());
                node->item->setValue(node->item->sum());
                node->item->setField(1, QString::number(node->item->sum()));
                parentNode = node;
            }
            if (ui->actionRelocationHeatmap->isChecked() && entry->sectionHeader()->flags() & SHF_WRITE)
                parentNode->item->setBackColor(relocColor(relocRatio(entry)));
        }
    }

    baseItem->setSorting(-2, true, true); // sort recursively by value
}

void SizeTreeMapView::treeMapContextMenu(const QPoint& pos)
{
    QMenu menu;
    m_treeMap->addSplitDirectionItems(&menu);
    menu.exec(m_treeMap->mapToGlobal(pos));

    QSettings settings;
    settings.setValue(QStringLiteral("TreeMap/SplitMode"), m_treeMap->splitModeString());
}

static void readCheckedState(QAction *action, bool def)
{
    const auto data = action->data().toString();
    if (data.isEmpty())
        return;
    QSettings settings;
    settings.beginGroup(QStringLiteral("View"));
    action->setChecked(settings.value(data, def).toBool());
}

void SizeTreeMapView::restoreSettings()
{
    readCheckedState(ui->actionHideDebugInformation, true);
    readCheckedState(ui->actionHideOccupiesMemory, false);
    readCheckedState(ui->actionHideNonExecutable, false);
    readCheckedState(ui->actionHideNonWritable, false);

    readCheckedState(ui->actionNoColorization, false);
    readCheckedState(ui->actionColorizeSections, false);
    readCheckedState(ui->actionColorizeSymbols, true);
    readCheckedState(ui->actionRelocationHeatmap, false);
}

void SizeTreeMapView::viewActionToggled()
{
    const auto action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    const auto data = action->data().toString();
    if (data.isEmpty())
        return;
    QSettings settings;
    settings.beginGroup(QStringLiteral("View"));
    settings.setValue(data, action->isChecked());

    // compress subsequent calls
    m_viewDirty = true;
    QMetaObject::invokeMethod(this, "reloadTreeMap", Qt::QueuedConnection);
}

bool SizeTreeMapView::isSectionHidden(ElfSectionHeader* shdr) const
{
    if (ui->actionHideDebugInformation->isChecked() && shdr->isDebugInformation())
        return true;
    if (ui->actionHideOccupiesMemory->isChecked() && !(shdr->flags() & SHF_ALLOC))
        return true;
    if (ui->actionHideNonWritable->isChecked() && !(shdr->flags() & SHF_WRITE))
        return true;
    if (ui->actionHideNonExecutable->isChecked() && !(shdr->flags() & SHF_EXECINSTR))
        return true;
    return false;
}
