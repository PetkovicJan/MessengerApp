#pragma once

#include <NetworkingCore/Client.h>

#include <NetworkingCore/MessengerDefines.h>

class MessengerClient : public Client
{
public:
  explicit MessengerClient(
    const char* ip_str, const char* port_str);

  void sendMessage(nlohmann::json const& msg);

private:
  virtual void onServerMessageReceived(std::string const& msg) override;
  virtual void onDisconnectedFromServer() override;

  virtual void onUserLoggedIn(int user_id, std::string const& username);
  virtual void onUserLoggedOut(int user_id);
  virtual void onUserMessageReceived(int user_id, std::string const& msg);
  virtual void onUserCreatedStatusReceived(CreateStatus status);
  virtual void onLoginStatusReceived(LoginStatus status);
};
