#include <NetworkingCore/UsersDatabase.h>

#include <iostream>
#include <format>

UsersDatabase::UsersDatabase(std::string const& databaseName)
{
  if (sqlite3_open(databaseName.c_str(), &m_database) != SQLITE_OK) {
    throw std::runtime_error(
      std::format("Users database failed to open: {}", sqlite3_errmsg(m_database)));
  }

  std::cout << "Database opened successfully\n";

  // SQL statement to create the users table, in case it does not already exist.
  // The table is named users and it has two columns: username and password, both
  // with values of text type. The username servers as the primary key, used when
  // retrieving the data.
  std::string const createUsersTableSql =
      "CREATE TABLE IF NOT EXISTS users ("
      "    username TEXT PRIMARY KEY,"
      "    password TEXT"
      ");";

  if (sqlite3_exec(m_database, createUsersTableSql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
    throw std::runtime_error(
      std::format("Failed to create users table: {}", sqlite3_errmsg(m_database)));
  }

  std::cout << "Table created successfully\n";
}

UsersDatabase::~UsersDatabase()
{
  if (m_database) sqlite3_close(m_database);
}

bool UsersDatabase::addUser(UserData const& userData)
{
  // This is the shorter way of doing it with exec() wrapper, instead of using primitives.
  // Note that this version, while being shorter, is vulnerable to SQL injection attacks,
  // since the values come from user input.

  // SQL statement to insert values under username and password column in the users table.
  std::string const inserUserSql = std::format(
    "INSERT INTO users (username, password) VALUES ('{}', '{}');", 
    userData.name, userData.passwordHash); 

  if (sqlite3_exec(m_database, inserUserSql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
    std::cout << std::format("Failed to insert user: {}\n", sqlite3_errmsg(m_database));
    return false;
  }

  std::cout << "User inserted successfully\n";

  return true;
}

std::optional<UserData> UsersDatabase::getUser(std::string const& userName)
{
  std::string const selectUserSql = 
  std::format("SELECT username, password FROM users WHERE username = '{}';", userName);

  auto const callback = [](
    void* dataPtr, int columnCount, char** columnString, char** columnNames)
  {
    // Fill the user structure with data, obtained from the query.
    UserData queriedUserData;
    for (int i = 0; i < columnCount; ++i) {
      if (columnNames[i] == std::string("username")) {
        queriedUserData.name = columnString[i];
      } else if (columnNames[i] == std::string("password")) {
        queriedUserData.passwordHash = columnString[i];
      }
    }

    // The first argument serves for passing additional data into the callback function.
    // In this case we use it to pass the (optional) user data struct instance.
    auto userData = reinterpret_cast<std::optional<UserData>*>(dataPtr);

    // Set the result to the "out" value.
    *userData = queriedUserData;

    return SQLITE_OK;
  };

  // Execute the select statement. Pass additional user data, that is subsequently passed to callback.
  std::optional<UserData> userData;
  if (sqlite3_exec(m_database, selectUserSql.c_str(), callback, nullptr, nullptr) != SQLITE_OK) {
    std::cout << std::format("Failed to retrieve user: {}\n", sqlite3_errmsg(m_database));

    return std::nullopt;
  }

  // In case that there is no corresponding user in the database with that name, the SQL statement still 
  // returns an OK status, since it is not an error, if the user is missing. But in that case, the callback
  // won't be invoked and the user will be nullopt, so the behavior is correct. 
  // TODO: Note that we could use fine-grained SQL methods to determine this directly.
  return userData;
}
