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
    const char* ip_str, const char* port_str, int max_num_clients);

private:
  void onClientConnected(int client_id) override;
  void onClientDisconnected(int client_id) override;
  void onClientMessageReceived(int client_id, std::string const& msg) override;

  void sendMessageToAllUsers(nlohmann::json const& msg, std::optional<int> opt_ignore_id = std::nullopt);

  // Users, that are currently logged in.
  std::vector<User> current_users_;

  // Registered users database.
  UsersDatabase users_db_;
};
