#pragma once

#include <QAbstractListModel>
#include <QList>

class UsersModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit UsersModel(QObject* parent = nullptr);

  void addUser(int id, QString const& name);

  void removeUser(int id);

  QString getUserName(int id) const;

  std::pair<int, QString> getUserAt(int row) const;

  QList<std::pair<int, QString>> getUsers() const;

private:
  int rowCount(QModelIndex const& index = QModelIndex()) const override;
  QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
  QList<std::pair<int, QString>> users_;
};
