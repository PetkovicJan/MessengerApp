#include "UsersModelQt.h"

UsersModel::UsersModel(QObject* parent) : QAbstractListModel(parent)
{
  users_.append(QString("Test user"));
}

int UsersModel::rowCount(QModelIndex const&) const
{
  return users_.size();
}

QVariant UsersModel::data(QModelIndex const& index, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  const int row = index.row();
  const bool is_in_range = row >= 0 && row < users_.size();
  if (!is_in_range)
    return QVariant();

  return users_.at(row);
}

QVariant UsersModel::headerData(
  int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation != Qt::Horizontal)
    return QVariant();

  return QString("Users");
}

bool UsersModel::insertRows(int row, int count, QModelIndex const& index)
{
  const int last_row = row + count - 1;
  beginInsertRows(QModelIndex(), row, last_row);

  for (int i = 0; i < count; ++i)
    users_.insert(row, QString());

  endInsertRows();

  return true;
}

bool UsersModel::removeRows(int row, int count, QModelIndex const& index)
{
  const int last_row = row + count - 1;
  beginRemoveRows(QModelIndex(), row, last_row);

  for (int i = 0; i < count; ++i)
    users_.removeAt(row);

  endRemoveRows();

  return true;
}

bool UsersModel::setData(
  QModelIndex const& index, QVariant const& value, int role)
{
  if (role != Qt::EditRole)
    return false;

  const int row = index.row();
  users_[row] = value.toString();

  emit dataChanged(index, index, { Qt::EditRole });

  return true;
}

Qt::ItemFlags UsersModel::flags(QModelIndex const& index) const
{
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

QList<QString> UsersModel::getUsers() const
{
  return users_;
}
