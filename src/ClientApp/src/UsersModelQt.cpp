#include <ClientApp/UsersModelQt.h>

#include <QFont>

#include <optional>

UsersModel::UsersModel(QObject* parent) : QAbstractListModel(parent)
{
}

void UsersModel::addUser(int id, QString const& name)
{
  const auto new_pos = users_.size();
  beginInsertRows(QModelIndex(), new_pos, new_pos);
  users_.push_back({ id, name, true });
  endInsertRows();
}

void UsersModel::removeUser(int id)
{
  auto it = std::find_if(users_.begin(), users_.end(),
    [id](UserData const& data) 
    {
      return data.id == id;
    });

  if (it == users_.end()) return;

  const auto row = std::distance(users_.begin(), it);

  beginRemoveRows(QModelIndex(), row, row);
  users_.erase(it);
  endRemoveRows();
}

QString UsersModel::getUserName(int id) const
{
  auto it = std::find_if(users_.begin(), users_.end(),
    [id](UserData const& data) 
    {
      return data.id == id;
    });

  if (it == users_.end()) return "";

  return it->name;
}

std::pair<int, QString> UsersModel::getUserAt(int row) const
{
  const auto& user = users_.at(row);
  return std::make_pair(user.id, user.name);
}

int UsersModel::rowCount(QModelIndex const&) const
{
  return users_.size();
}

QVariant UsersModel::data(QModelIndex const& index, int role) const
{
  const int row = index.row();

  // Check range.
  if (row < 0 || row >= users_.size()) return QVariant();

  const auto& user = users_.at(row);

  if (role == Qt::DisplayRole)
  {
    return user.name;
  }
  else if (role == Qt::FontRole)
  {
    QFont font;
    font.setBold(!user.checked);

    return font;
  }

  return QVariant();
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

void UsersModel::setChecked(int user_id, bool checked)
{
  auto it = std::find_if(users_.begin(), users_.end(),
    [user_id](UserData const& data) 
    {
      return data.id == user_id;
    });

  if (it == users_.end()) return;

  it->checked = checked;

  const auto row = std::distance(users_.begin(), it);
  const auto index = this->index(row, 0);
  emit dataChanged(index, index);
}
