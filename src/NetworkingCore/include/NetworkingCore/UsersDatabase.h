#pragma once

#include "sqlite3.h"

#include <string>
#include <optional>

struct UserData
{
  std::string name;
  std::string password_hash;
};

class UsersDatabase
{
public:
  explicit UsersDatabase(std::string const& db_name);
  ~UsersDatabase();

  bool addUser(UserData const& user);

  std::optional<UserData> getUser(std::string const& name);

private:
  sqlite3* db_ = nullptr;
};
