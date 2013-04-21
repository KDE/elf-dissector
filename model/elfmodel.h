#ifndef ELFMODEL_H
#define ELFMODEL_H

#include "elfnodevariant.h"

#include <QAbstractItemModel>

class ElfFileSet;

/** Model for the ELF structure.
 *
 * Internal pointer is the ElfNodeVariant pointer containing the _parent_ for the node.
 */
class ElfModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Role {
        SizeRole = Qt::UserRole + 1,
        DetailRole
    };

    explicit ElfModel(QObject* parent);
    ~ElfModel();

    void setFileSet(ElfFileSet* fileSet);

    QVariant data(const QModelIndex& index, int role) const override;
    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    void clearInternalPointerMap();
    ElfNodeVariant* variantForIndex(const QModelIndex &index) const;
    ElfNodeVariant contentForIndex(const QModelIndex& index) const;
    ElfNodeVariant* makeVariant(void* payload, ElfNodeVariant::Type type) const;

private:
    ElfFileSet *m_fileSet = 0;
    mutable QHash<void*, ElfNodeVariant*> m_internalPointerMap;

};

#endif // ELFMODEL_H
