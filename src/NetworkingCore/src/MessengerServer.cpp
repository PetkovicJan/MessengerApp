#include <NetworkingCore/MessengerServer.h>

#include <picosha2.h>

#include <iostream>

#define ADD_DEFAULT_USERS

using json = nlohmann::json;

namespace detail
{
  std::string get_hash(std::string const& str)
  {
    std::string str_hash(picosha2::k_digest_size, '*');
    picosha2::hash256(str, str_hash);

    return str_hash;
  }
}

MessengerServer::MessengerServer(
  const char* ip_str, const char* port_str, int max_num_clients) :
  Server(ip_str, port_str, max_num_clients), users_db_("users.db")
{
#ifdef ADD_DEFAULT_USERS
  users_db_.addUser({ "user1", detail::get_hash("abc123") });
  users_db_.addUser({ "user2", detail::get_hash("def456") });
  users_db_.addUser({ "user3", detail::get_hash("ghi789") });
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
  std::cout << "Server::onClientMessageReceived: " << app_msg << '\n';

  const auto msg_type = app_msg["type"].get<AppMessageType>();
  if (msg_type == AppMessageType::UserCreated)
  {
    const auto username = app_msg["username"].get<std::string>();
    const auto password = app_msg["password"].get<std::string>();

    // Password hash needs to be stored instead of password.
    const auto password_hash = detail::get_hash(password);

    // Add user to database.
    UserData user{ username, password_hash };
    const auto success = users_db_.addUser(user);

    json user_created_msg;
    user_created_msg["type"] = AppMessageType::UserCreatedStatus;
    user_created_msg["status"] = success ? CreateStatus::Success : CreateStatus::Failure;
    sendMessageToClient(client_id, user_created_msg.dump());
  }
  else if (msg_type == AppMessageType::UserLoggedIn)
  {
    const auto username = app_msg["username"].get<std::string>();
    const auto password = app_msg["password"].get<std::string>();

    // First authenticate the user.
    const auto user = users_db_.getUser(username);
    if (!user.has_value())
    {
      json login_msg;
      login_msg["type"] = AppMessageType::UserLoginStatus;
      login_msg["status"] = LoginStatus::InvalidUsername;
      sendMessageToClient(client_id, login_msg.dump());
      return;
    }

    // Get password hash to compare with the stored one.
    const auto password_hash = detail::get_hash(password);
    if (user->password_hash != password_hash)
    {
      json login_msg;
      login_msg["type"] = AppMessageType::UserLoginStatus;
      login_msg["status"] = LoginStatus::InvalidPassword;
      sendMessageToClient(client_id, login_msg.dump());
      return;
    }

    // Authentication succeeded, notify the user.
    json login_msg;
    login_msg["type"] = AppMessageType::UserLoginStatus;
    login_msg["status"] = LoginStatus::Success;
    sendMessageToClient(client_id, login_msg.dump());

    // Then notify the new user of all users, that are currently logged in.
    for (const auto& user : current_users_)
    {
      // Create a logged in message.
      json notify_msg;
      notify_msg["type"] = AppMessageType::UserLoggedIn;
      notify_msg["user_id"] = user.id;
      notify_msg["username"] = user.name;
      sendMessageToClient(client_id, notify_msg.dump());
    }

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
