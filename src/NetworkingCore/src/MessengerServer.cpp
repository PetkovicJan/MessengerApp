#include <NetworkingCore/MessengerServer.h>

#include <picosha2.h>

#include <iostream>
#include <format>
#include <algorithm>
#include <ranges>

// Useful for testing in order to avoid registering new users.
#define ADD_DEFAULT_USERS

using json = nlohmann::json;

namespace detail
{
  std::string computeHash(std::string const& str)
  {
    std::string hash(picosha2::k_digest_size, ' ');

    picosha2::hash256(str, hash);

    return hash;
  }
}

MessengerServer::MessengerServer(
  const char* ipStr, const char* portStr, int maxNumClients) :
  Server(ipStr, portStr, maxNumClients), m_UsersDatabase("users.db")
{
#ifdef ADD_DEFAULT_USERS
  m_UsersDatabase.addUser({ "user1", detail::computeHash("abc123") });
  m_UsersDatabase.addUser({ "user2", detail::computeHash("def456") });
  m_UsersDatabase.addUser({ "user3", detail::computeHash("ghi789") });
#endif
}

void MessengerServer::onClientConnected(int clientId)
{
  // At this point nothing needs to be done, since the client has only 
  // connected to the server, but has not yet logged in.
  std::cout << std::format("Server::onClientConnected: {}\n", clientId);
}

void MessengerServer::onClientDisconnected(int clientId)
{
  std::cout << std::format("Server::onClientDisconnected: {}\n", clientId);

  auto userIt = std::ranges::find(m_loggedInUsers, clientId, &User::id);
  if (userIt == m_loggedInUsers.end()) {
    // User hasn't logged in yet, nothing to do.
    return;
  }

  m_loggedInUsers.erase(userIt);

  json msg;
  msg["type"] = AppMessageType::UserLoggedOut;
  msg["user_id"] = clientId;

  sendMessageToAllUsers(msg, clientId);
}

void MessengerServer::onClientMessageReceived(
  int clientId, std::string const& msgStr)
{
  auto const appMsg = json::parse(msgStr);
  std::cout << std::format("Server::onClientMessageReceived: {}\n", appMsg.dump());

  AppMessageType const msgType = appMsg["type"].get<AppMessageType>();

  if (msgType == AppMessageType::UserCreated) {
    std::string username = appMsg["username"].get<std::string>();
    std::string password = appMsg["password"].get<std::string>();

    // Password hash is stored instead of password itself.
    std::string passwordHash = detail::computeHash(password);

    UserData const userData{ 
      .name = std::move(username),
      .passwordHash = std::move(passwordHash),
    };

    // Store user to database.
    bool const storeSuccess = m_UsersDatabase.addUser(userData);

    json userCreatedMsg;
    userCreatedMsg["type"] = AppMessageType::UserCreatedStatus;
    userCreatedMsg["status"] = storeSuccess ? CreateStatus::Success : CreateStatus::Failure;

    sendMessageToClient(clientId, userCreatedMsg.dump());
  } 
  else if (msgType == AppMessageType::UserLoggedIn) {
    std::string username = appMsg["username"].get<std::string>();
    std::string password = appMsg["password"].get<std::string>();

    // Fetch user from database.
    std::optional<UserData> const user = m_UsersDatabase.getUser(username);

    if (!user.has_value()) {
      json loginMsg;
      loginMsg["type"] = AppMessageType::UserLoginStatus;
      loginMsg["status"] = LoginStatus::InvalidUsername;

      sendMessageToClient(clientId, loginMsg.dump());

      return;
    }

    // Check password.
    std::string const passwordHash = detail::computeHash(password);
    if (user->passwordHash != passwordHash) {
      json loginMsg;
      loginMsg["type"] = AppMessageType::UserLoginStatus;
      loginMsg["status"] = LoginStatus::InvalidPassword;

      sendMessageToClient(clientId, loginMsg.dump());

      return;
    }

    // Authentication successful, notify the newly logged in user.
    json loginMsg;
    loginMsg["type"] = AppMessageType::UserLoginStatus;
    loginMsg["status"] = LoginStatus::Success;

    sendMessageToClient(clientId, loginMsg.dump());

    // Notify the newly logged in user of all users, that are already logged in.
    for (const auto& user : m_loggedInUsers) {
      json msg;
      msg["type"] = AppMessageType::UserLoggedIn;
      msg["user_id"] = user.id;
      msg["username"] = user.name;

      sendMessageToClient(clientId, msg.dump());
    }

    // Update the currently logged in users.
    m_loggedInUsers.push_back({ clientId, username });

    // When the message comes from the client, it only contains its username, but *not the ID*, 
    // since it is implicit on the server side. However, when we send the notification message
    // to other clients, we need to add the ID, so they know which client logged in.

    // Notify users that are already logged in about the newly logged in user.
    json newUserMsg;
    newUserMsg["type"] = AppMessageType::UserLoggedIn;
    newUserMsg["user_id"] = clientId;
    newUserMsg["username"] = username;

    sendMessageToAllUsers(newUserMsg, clientId);
  }
  else if (msgType == AppMessageType::UserSentMessage) {
    // Obtain the receiving client ID and message.
    int const clientIdTo = appMsg["user_id"].get<int>();
    std::string const msg = appMsg["message"].get<std::string>();

    int const clientIdFrom = clientId;

    // Create a new message with the sender client ID and message.
    json newMsg; 
    newMsg["type"] = AppMessageType::UserSentMessage;
    newMsg["user_id"] = clientIdFrom;
    newMsg["message"] = msg;

    // Send new message to the receiving client.
    sendMessageToClient(clientIdTo, newMsg.dump());
  }
}

void MessengerServer::sendMessageToAllUsers(json const& msg, std::optional<int> ignoreId)
{
  std::string const serializedMsg = msg.dump();

  auto const sendMsg = [&](int userId) {
    sendMessageToClient(userId, serializedMsg);
  };

  auto filteredUsers = m_loggedInUsers | std::views::filter([&](User const& user) {
    return !(ignoreId && *ignoreId == user.id);
  });

  std::ranges::for_each(filteredUsers, sendMsg, &User::id);
}
