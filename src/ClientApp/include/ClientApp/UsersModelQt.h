#pragma once

#include <QAbstractListModel>

#include <vector>

class UsersModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit UsersModel(QObject* parent = nullptr);

  void addUser(int id, QString const& name);

  void removeUser(int id);

  QString getUserName(int id) const;

  std::pair<int, QString> getUserAt(int row) const;

  void setChecked(int user_id, bool checked);

private:
  // Virtual functions to override.
  int rowCount(QModelIndex const& index = QModelIndex()) const override;
  QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  struct UserData
  {
    int id = -1;
    QString name;
    bool checked = false;
  };

  std::vector<UserData> users_;
};
