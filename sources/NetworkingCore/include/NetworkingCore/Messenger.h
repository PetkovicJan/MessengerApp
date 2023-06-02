#pragma once

#include "Server.h"
#include "Client.h"
#include "UsersDB.h"

#include <nlohmann/json.hpp>

#include <thread>
#include <mutex>
#include <atomic>

using json = nlohmann::json;

enum class AppMessageType { UserLoggedIn, UserLoggedOut, UserSentMessage, UserLoginStatus };

enum class LoginStatus { Success, InvalidUsername, InvalidPassword };

struct User
{
  int id;
  std::string name;
};

class MessengerServer : public Server
{
public:
  explicit MessengerServer(
    std::string const& ip_str, std::string const& port_str, int max_num_clients);

private:
  void onClientConnected(int client_id) override;
  void onClientDisconnected(int client_id) override;
  void onClientMessageReceived(int client_id, std::string const& msg) override;

  void sendMessageToAllUsers(json const& msg, std::optional<int> opt_ignore_id = std::nullopt);

  // Users, that are currently logged in.
  std::vector<User> current_users_;

  // Registered users database.
  UsersDB users_db_;
};

class MessengerClient : public Client
{
public:
  explicit MessengerClient(
    std::string const& ip_str, std::string const& port_str);

  void sendMessage(json const& msg);

private:
  virtual void onServerMessageReceived(std::string const& msg) override;
  virtual void onDisconnectedFromServer() override;

  virtual void onUserLoggedIn(int user_id, std::string const& username);
  virtual void onUserLoggedOut(int user_id);
  virtual void onUserMessageReceived(int user_id, std::string const& msg);
  virtual void onLoginStatusReceived(LoginStatus status);
};
