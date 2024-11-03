#pragma once

#include <NetworkingCore/Server.h>

#include <NetworkingCore/MessengerDefines.h>
#include <NetworkingCore/UsersDatabase.h>

#include <nlohmann/json.hpp>

struct User
{
  int id;
  std::string name;
};

class MessengerServer : public Server
{
public:
  explicit MessengerServer(
    const char* ipStr, const char* portStr, int maxNumClients);

private:
  void onClientConnected(int clientId) override;
  void onClientDisconnected(int clientId) override;
  void onClientMessageReceived(int clientId, std::string const& msg) override;

  void sendMessageToAllUsers(
    nlohmann::json const& msg, std::optional<int> ignoreId = std::nullopt);

  // Users, that are currently logged in.
  std::vector<User> m_loggedInUsers;

  // Registered users database.
  UsersDatabase m_UsersDatabase;
};
