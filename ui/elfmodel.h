#ifndef ELFMODEL_H
#define ELFMODEL_H

#include <QAbstractItemModel>

#include <elf/elffile.h>

class ElfFile;

class ElfModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ElfModel(QObject* parent);
    ~ElfModel();

    void setFile(const ElfFile::Ptr &file);

    QVariant data(const QModelIndex& index, int role) const override;
    int columnCount(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    ElfFile::Ptr m_file;

};

#endif // ELFMODEL_H
