#pragma once

#include "sqlite3.h"

#include <string>
#include <optional>

struct UserData
{
  std::string name;
  std::string password;
};

class UsersDB
{
public:
  explicit UsersDB(std::string const& db_name);
  ~UsersDB();

  bool addUser(UserData const& user);

  std::optional<UserData> getUser(std::string const& name);

private:
  sqlite3* db_ = nullptr;
};
