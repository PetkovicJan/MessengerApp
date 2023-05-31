#include <NetworkingCore/UsersDB.h>

#include <iostream>

UsersDB::UsersDB(std::string const& db_name)
{
  if (sqlite3_open("users.db", &db_) != SQLITE_OK)
  {
    throw std::runtime_error(std::string("Users database failed to open: ") + sqlite3_errmsg(db_));
  }

  std::cout << "Database opened successfully\n";

  // SQL statement to create the users table, in case it does not already exist.
  // The table is named users and it has two columns: username and password, both
  // with values of text type. The username servers as the primary key, used when
  // retrieving the data.
  const char* create_users_table_sql =
      "CREATE TABLE IF NOT EXISTS users ("
      "    username TEXT PRIMARY KEY,"
      "    password TEXT"
      ");";

  if (sqlite3_exec(db_, create_users_table_sql, nullptr, nullptr, nullptr) != SQLITE_OK)
  {
    throw std::runtime_error(std::string("Failed to create users table: ") + sqlite3_errmsg(db_));
  }

  std::cout << "Table created successfully\n";
}

UsersDB::~UsersDB()
{
  if (db_) sqlite3_close(db_);
}

bool UsersDB::addUser(UserData const& user)
{
  // This is the shorter way of doing it with exec() wrapper, instead of using primitives.
  // Note that this version, while being shorter, is vulnerable to SQL injection attacks,
  // since the values come from user input.

  // SQL statement to insert values under username and password column in the users table.
  std::string insert_user_sql = "INSERT INTO users (username, password) VALUES ('" +
    std::string(user.name) + "', '" + std::string(user.password) + "');";

  if (sqlite3_exec(db_, insert_user_sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK)
  {
    std::cout << "Failed to insert user: " << sqlite3_errmsg(db_) << '\n';
    return false;
  }

  std::cout << "User inserted successfully\n";

  return true;
}

std::optional<UserData> UsersDB::getUser(std::string const& name)
{
  const auto select_user_sql = "SELECT username, password FROM users WHERE username = '" + name + "';";

  auto callback = [](void* user_ptr, int column_count, char** column_strings, char** column_names)
  {
    // The first argument server for passing additional data into the callback function.
    // In this case, this is the user data struct instance.
    auto user = reinterpret_cast<UserData*>(user_ptr);

    for (int i = 0; i < column_count; ++i)
    {
      if (column_names[i] == std::string("username"))
        user->name = column_strings[i];
      else if (column_names[i] == std::string("password"))
        user->password = column_strings[i];
    }

    for (int i = 0; i < column_count; ++i)
    {
      std::cout << column_names[i] << ": " << column_strings[i] << '\n';
    }

    return SQLITE_OK;
  };

  // Execute the select statement. Pass additional user data, that is subsequently passed to callback.
  UserData user;
  if (sqlite3_exec(db_, select_user_sql.c_str(), callback, &user, nullptr) != SQLITE_OK)
  {
    std::cout << "Failed to retrieve user: " << sqlite3_errmsg(db_) << '\n';
    return std::nullopt;
  }

  std::cout << "User retrieved successfully\n";

  return user;
}
