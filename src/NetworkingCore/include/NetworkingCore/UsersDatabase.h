#pragma once

#include "sqlite3.h"

#include <string>
#include <optional>

struct UserData
{
  std::string name;
  std::string passwordHash;
};

class UsersDatabase
{
public:
  explicit UsersDatabase(std::string const& databaseName);
  ~UsersDatabase();

  bool addUser(UserData const& userData);

  std::optional<UserData> getUser(std::string const& userName);

private:
  sqlite3* m_database = nullptr;
};
