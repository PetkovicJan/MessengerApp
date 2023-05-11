#pragma once

#include "Socket.h"

#include <atomic>
#include <memory>

class Client
{
public:
  explicit Client(
    std::string const& ip_str, std::string const& port_str);
  virtual ~Client();

  void exec();

  void stopClient();

  void sendMessageToServer(std::string const& msg);

private:
  // Virtual functions, that should be overridden by derived classes.
  virtual void onServerMessageReceived(std::string const& msg);
  virtual void onDisconnectedFromServer();

  SocketContext context_;

  std::atomic<bool> is_alive_ = true;
  std::unique_ptr<ConnectedSocket> server_;
};
