#include "Messenger.h"

#include <iostream>

AppMessage::AppMessage(AppMessageType type, std::optional<std::string> const& opt_data)
{
  header_.type = type;

  if (opt_data)
    data_ = *opt_data;

  header_.size = sizeof(MessageHeader) + data_.size();
}

AppMessageType AppMessage::type() const
{
  return header_.type;
}

std::string AppMessage::data() const
{
  return data_;
}

std::string serialize(AppMessage const& msg)
{
  auto const header_sz = sizeof(AppMessage::MessageHeader);

  auto const& header = msg.header_;
  const auto& data = msg.data_;

  // Create an array of an appropriate size.
  std::string res;
  res.resize(header.size, 'x');

  // First copy the header part.
  std::memcpy(res.data(), &header, header_sz);

  // Then copy the data part.
  std::memcpy(res.data() + header_sz, data.data(), data.size());

  return res;
}

AppMessage deserialize(std::string const& str)
{
  auto const header_sz = sizeof(AppMessage::MessageHeader);

  // First read the header part.
  AppMessage::MessageHeader header;
  std::memcpy(&header, str.data(), header_sz);

  // Now read the data, since we know the size of the message.
  std::string data;
  auto const data_sz = header.size - header_sz;
  if (data_sz > 0)
  {
    data.resize(data_sz, 'x');
    std::memcpy(data.data(), str.data() + header_sz, data_sz);
  }

  return AppMessage(header.type, data);
}

template<>
AppMessage& operator << (AppMessage& msg, std::string const& put_data)
{
  msg.data_.append(put_data);

  return msg;
}

template<>
AppMessage& operator >> (AppMessage& msg, std::string& take_data)
{
  take_data = std::move(msg.data_);

  return msg;
}

MessengerServer::MessengerServer(
  std::string const& ip_str, std::string const& port_str, int max_num_clients) :
  Server(ip_str, port_str, max_num_clients)
{}

void MessengerServer::onClientConnected(int client_id)
{
  AppMessage msg(AppMessageType::UserLoggedIn, std::to_string(client_id));
  sendMessageToAllClients(serialize(msg), client_id);
}

void MessengerServer::onClientDisconnected(int client_id)
{
  AppMessage msg(AppMessageType::UserLoggedOut, std::to_string(client_id));
  sendMessageToAllClients(serialize(msg), client_id);
}

void MessengerServer::onClientMessageReceived(
  int client_id, std::string const& msg_str)
{
  auto app_msg = deserialize(msg_str);
  if (app_msg.type() == AppMessageType::UserSentMessage)
  {
    // Obtain the receiving client ID and message.
    int client_to;
    std::string msg;
    app_msg >> client_to >> msg;

    // Create a new message with the sender client ID and message.
    const int client_from = client_id;
    AppMessage new_msg(AppMessageType::UserSentMessage);
    new_msg << client_from << msg;

    // Send new message to the receiving client.
    sendMessageToClient(client_to, serialize(new_msg));
  }
}

MessengerClient::MessengerClient(
  std::string const& ip_str, std::string const& port_str) :
  Client(ip_str, port_str)
{}

void MessengerClient::sendMessageToUser(int client_to, std::string const& msg)
{
    AppMessage new_msg(AppMessageType::UserSentMessage);
    new_msg << client_to << msg;
    sendMessageToServer(serialize(new_msg));
}

void MessengerClient::onServerMessageReceived(std::string const& data)
{
  auto app_msg = deserialize(data);

  auto const msg_t = app_msg.type();
  if (msg_t == AppMessageType::UserLoggedIn)
  {
    int user_id;
    std::string user_name;
    app_msg >> user_id >> user_name;
    onUserLoggedIn(user_id, user_name);
  }
  else if (msg_t == AppMessageType::UserLoggedOut)
  {
    int user_id;
    app_msg >> user_id;
    onUserLoggedOut(user_id);
  }
  else if (msg_t == AppMessageType::UserSentMessage)
  {
    int user_id;
    std::string user_msg;
    app_msg >> user_id >> user_msg;
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

bool are_same(AppMessage const& one, AppMessage const& other)
{
  return (one.type() == other.type()) && (one.data() == other.data());
}

void test_case(std::string const& test_name, AppMessage const& msg)
{
  auto const serialized = serialize(msg);
  auto const deserialized = deserialize(serialized);
  const bool same = are_same(msg, deserialized);
  std::cout << test_name << ": " << (same ? "Success" : "Failure") << std::endl;
}

void test_message_serialization()
{
  {
    AppMessage msg(AppMessageType::UserLoggedIn);
    test_case("Case 1", msg);
  }

  {
    AppMessage msg(AppMessageType::UserLoggedOut);
    test_case("Case 2", msg);
  }

  {
    auto const data = std::to_string(1);
    AppMessage msg(AppMessageType::UserLoggedIn, data);
    test_case("Case 3", msg);
  }

  {
    auto const data = "User message.";
    AppMessage msg(AppMessageType::UserSentMessage, data);
    test_case("Case 4", msg);
  }
}

void test_message_streaming()
{
  AppMessage msg(AppMessageType::UserLoggedIn);
  int a = 4;
  bool b = true;
  float c = 7.f;
  std::string s = "Foo";
  msg << a << b << c << s;
  
  int a1;
  bool b1;
  float c1;
  std::string s1;
  msg >> a1 >> b1 >> c1 >> s1;

  const bool same = a == a1 && b == b1 && c == c1 && s == s1;
  std::cout << "Streaming test" << ": " << (same ? "Success" : "Failure") << std::endl;
}

