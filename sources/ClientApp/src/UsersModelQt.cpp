#include <ClientApp/UsersModelQt.h>

UsersModel::UsersModel(QObject* parent) : QAbstractListModel(parent)
{
}

void UsersModel::addUser(int id, QString const& name)
{
  const auto new_pos = users_.size();
  beginInsertRows(QModelIndex(), new_pos, new_pos);
  users_.insert(new_pos, std::make_pair(id, name));
  endInsertRows();
}

void UsersModel::removeUser(int id)
{
  // Find the item with the given ID.
  int id_index = -1;
  for (int i = 0; i < users_.size(); ++i)
    if (users_.at(i).first == id)
    {
      id_index = i;
      break;
    }

  if (id_index == -1) return;

  beginRemoveRows(QModelIndex(), id_index, id_index);
  users_.removeAt(id_index);
  endRemoveRows();
}

QString UsersModel::getUserName(int id) const
{
  // Find the item with the given ID.
  int id_index = -1;
  for (int i = 0; i < users_.size(); ++i)
    if (users_.at(i).first == id)
    {
      id_index = i;
      break;
    }

  if (id_index == -1) return "";

  return users_.at(id_index).second;
}

std::pair<int, QString> UsersModel::getUserAt(int row) const
{
  return users_.at(row);
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

  return users_.at(row).second;
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

QList<std::pair<int, QString>> UsersModel::getUsers() const
{
  return users_;
}
