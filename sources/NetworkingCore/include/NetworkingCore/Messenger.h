#pragma once

#include "Server.h"
#include "Client.h"

#include <thread>
#include <mutex>
#include <atomic>

enum class AppMessageType { UserSentMessage, UserLoggedIn, UserLoggedOut };

class AppMessage
{
public:
  explicit AppMessage(
    AppMessageType type, 
    std::optional<std::string> const& opt_data = std::nullopt);

  AppMessageType type() const;

  std::string data() const;

  template<typename DataT>
  friend AppMessage& operator << (AppMessage& msg, DataT const& put_data);

  template<typename DataT>
  friend AppMessage& operator >> (AppMessage& msg, DataT& take_data);

private:
  struct MessageHeader
  {
    AppMessageType type;
    int size;
  } header_;

  std::string data_;

  friend std::string serialize(AppMessage const&);
  friend AppMessage deserialize(std::string const&);
};

template<typename DataT>
AppMessage& operator<< (AppMessage& msg, DataT const& put_data)
{
  static_assert(std::is_standard_layout_v<DataT>);

  auto& data = msg.data_;

  auto const curr_sz = data.size();
  auto const put_data_sz = sizeof(DataT);
  data.resize(curr_sz + put_data_sz);

  std::memcpy(data.data() + curr_sz, &put_data, put_data_sz);

  return msg;
}

template<typename DataT>
AppMessage& operator>> (AppMessage& msg, DataT& take_data)
{
  static_assert(std::is_standard_layout_v<DataT>);

  auto& data = msg.data_;

  auto const curr_sz = data.size();
  auto const take_data_sz = sizeof(DataT);
  auto const new_sz = curr_sz - take_data_sz;

  std::memcpy(&take_data, data.data(), take_data_sz);

  data = data.substr(take_data_sz, new_sz);

  return msg;
}

// Add template specialization for strings. Note that in case of reading 
// a string from message, we choose to simply take the whole string, since 
// we don't know how long it should be.
template<>
AppMessage& operator<<(AppMessage& msg, std::string const& put_data);
template<>
AppMessage& operator>>(AppMessage& msg, std::string& take_data);

void test_message_serialization();
void test_message_streaming();

class MessengerServer : public Server
{
public:
  explicit MessengerServer(
    std::string const& ip_str, std::string const& port_str, int max_num_clients);

private:
  void onClientConnected(int client_id) override;
  void onClientDisconnected(int client_id) override;
  void onClientMessageReceived(int client_id, std::string const& msg) override;
};

class MessengerClient : public Client
{
public:
  explicit MessengerClient(
    std::string const& ip_str, std::string const& port_str);

  void sendMessage(AppMessage const& msg);

private:
  virtual void onServerMessageReceived(std::string const& msg) override;
  virtual void onDisconnectedFromServer() override;

  virtual void onUserLoggedIn(int user_id, std::string const& username);
  virtual void onUserLoggedOut(int user_id);
  virtual void onUserMessageReceived(int user_id, std::string const& msg);
};