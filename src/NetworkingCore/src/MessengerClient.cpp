#include <NetworkingCore/MessengerClient.h>

#include <iostream>

using json = nlohmann::json;

MessengerClient::MessengerClient(
  const char* ip_str, const char* port_str) :
  Client(ip_str, port_str)
{}

void MessengerClient::sendMessage(json const& msg)
{
  sendMessageToServer(msg.dump());
}

void MessengerClient::onServerMessageReceived(std::string const& data)
{
  const auto app_msg = json::parse(data);
  std::cout << "Client::onServerMessageReceived: " << app_msg << '\n';

  auto const msg_t = app_msg["type"].get<AppMessageType>();
  if (msg_t == AppMessageType::UserLoggedIn)
  {
    const auto user_id = app_msg["user_id"].get<int>();
    const auto user_name = app_msg["username"].get<std::string>();
    onUserLoggedIn(user_id, user_name);
  }
  else if (msg_t == AppMessageType::UserLoggedOut)
  {
    const auto user_id = app_msg["user_id"].get<int>();
    onUserLoggedOut(user_id);
  }
  else if (msg_t == AppMessageType::UserSentMessage)
  {
    const auto user_id = app_msg["user_id"].get<int>();
    const auto user_msg = app_msg["message"].get<std::string>();
    onUserMessageReceived(user_id, user_msg);
  }
  if (msg_t == AppMessageType::UserCreatedStatus)
  {
    const auto status = app_msg["status"].get<CreateStatus>();
    onUserCreatedStatusReceived(status);
  }
  else if (msg_t == AppMessageType::UserLoginStatus)
  {
    const auto status = app_msg["status"].get<LoginStatus>();
    onLoginStatusReceived(status);
  }
}

void MessengerClient::onDisconnectedFromServer()
{
}

void MessengerClient::onUserLoggedIn(int user_id, std::string const& user_name)
{
}

void MessengerClient::onUserLoggedOut(int user_id)
{
}

void MessengerClient::onUserMessageReceived(int user_id, std::string const& msg)
{
}

void MessengerClient::onUserCreatedStatusReceived(CreateStatus status)
{
}

void MessengerClient::onLoginStatusReceived(LoginStatus status)
{
}
