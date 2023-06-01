#include <NetworkingCore/Messenger.h>

#include <iostream>

#define ADD_DEFAULT_USERS

MessengerServer::MessengerServer(
  std::string const& ip_str, std::string const& port_str, int max_num_clients) :
  Server(ip_str, port_str, max_num_clients), users_db_("users.db")
{
#ifdef ADD_DEFAULT_USERS
  users_db_.addUser({ "Jan", "jan123" });
  users_db_.addUser({ "Nika", "nika345" });
  users_db_.addUser({ "Bor", "bor678" });
  //users_db_.getUser("Jan");
  //users_db_.getUser("Bor");
#endif
}

void MessengerServer::onClientConnected(int client_id)
{
  // At this point nothing needs to be done, since the client has only connected to the server, but has not yet logged in.
  std::cout << "Server::onClientConnected: " << client_id << '\n';
}

void MessengerServer::onClientDisconnected(int client_id)
{
  std::cout << "Server::onClientDisconnected: " << client_id << '\n';

  // First remove the user from the local current users.
  auto it = std::find_if(current_users_.begin(), current_users_.end(),
    [client_id](User const& user)
    {
      return user.id == client_id;
    });

  // Sanity check.
  if (it != current_users_.end())
    current_users_.erase(it);

  // Finally, notify the rest of the users.
  json msg;
  msg["type"] = AppMessageType::UserLoggedOut;
  msg["user_id"] = client_id;
  sendMessageToAllUsers(msg, client_id);
}

void MessengerServer::onClientMessageReceived(
  int client_id, std::string const& msg_str)
{
  const auto app_msg = json::parse(msg_str);
  const auto msg_type = app_msg["type"].get<AppMessageType>();

  std::cout << "Server::onClientMessageReceived: " << app_msg << '\n';

  if (msg_type == AppMessageType::UserLoggedIn)
  {
    // First notify the new user of all users, that are currently logged in.
    for (const auto& user : current_users_)
    {
      // Create a logged in message.
      json notify_msg;
      notify_msg["type"] = AppMessageType::UserLoggedIn;
      notify_msg["user_id"] = user.id;
      notify_msg["username"] = user.name;
      sendMessageToClient(client_id, notify_msg.dump());
    }

    // Then obtain the username of the logged in client.
    const auto username = app_msg["username"].get<std::string>();

    // Update the current users.
    current_users_.push_back({ client_id, username });

    // When the message comes from the client, it only contains its username, but *not the ID*, 
    // since it is implicit on the server side. However, when we send the notification message
    // to other clients, we need to add the ID, so they know which client logged in.
    json new_msg;
    new_msg["type"] = AppMessageType::UserLoggedIn;
    new_msg["user_id"] = client_id;
    new_msg["username"] = username;

    // Finally, inform the other users about the new user.
    sendMessageToAllUsers(new_msg, client_id);
  }
  else if (msg_type == AppMessageType::UserSentMessage)
  {
    // Obtain the receiving client ID and message.
    const auto client_to = app_msg["user_id"].get<int>();
    const auto msg = app_msg["message"].get<std::string>();

    // Create a new message with the sender client ID and message.
    const int client_from = client_id;
    json new_msg; 
    new_msg["type"] = AppMessageType::UserSentMessage;
    new_msg["user_id"] = client_from;
    new_msg["message"] = msg;

    // Send new message to the receiving client.
    sendMessageToClient(client_to, new_msg.dump());
  }
}

void MessengerServer::sendMessageToAllUsers(json const& msg, std::optional<int> opt_ignore_id)
{
  const auto serialized_msg = msg.dump();
  for (auto const& user : current_users_)
  {
    if (opt_ignore_id && *opt_ignore_id == user.id) 
      continue;

    sendMessageToClient(user.id, serialized_msg);
  }
}

MessengerClient::MessengerClient(
  std::string const& ip_str, std::string const& port_str) :
  Client(ip_str, port_str)
{}

void MessengerClient::sendMessage(json const& msg)
{
  sendMessageToServer(msg.dump());
}

void MessengerClient::onServerMessageReceived(std::string const& data)
{
  const auto app_msg = json::parse(data);

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
