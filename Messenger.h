#pragma once

#include "Socket.h"

#include <thread>
#include <mutex>
#include <atomic>

class MessengerApp
{
public:
  explicit MessengerApp(ConnectedSocket& socket);

  void start();

private:
  std::string queryMessage();

  void displayMessage(std::string const& msg);

  ConnectedSocket& socket_;

  std::atomic<bool> stop_flag_ = true;

  std::thread receiver_;

  std::mutex output_mtx_;
};};

enum class AppMessageType { UserSentMessage, UserLoggedIn, UserLoggedOut };

class AppMessage
{
public:
  explicit AppMessage(
    AppMessageType type, 
    std::optional<std::string> const& opt_data = std::nullopt);

  AppMessageType type() const;

  std::string data() const;

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
