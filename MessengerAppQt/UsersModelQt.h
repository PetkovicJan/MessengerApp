#pragma once

#include <QAbstractListModel>
#include <QList>

class UsersModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit UsersModel(QObject* parent = nullptr);

  int rowCount(QModelIndex const& index = QModelIndex()) const override;
  QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  bool insertRows(int row, int count, QModelIndex const& index = QModelIndex()) override;
  bool removeRows(int row, int count, QModelIndex const& index = QModelIndex()) override;
  bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole) override;

  Qt::ItemFlags flags(QModelIndex const& index) const override;

  QList<QString> getUsers() const;

private:
  QList<QString> users_;
};
