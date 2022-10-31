#pragma once

#include "Socket.h"
#include "ThreadSafeQueue.h"

#include <thread>
#include <atomic>

class Connection
{
public:
  Connection(ConnectedSocket&& socket, MessageQueue& message_queue);
  ~Connection();

  bool isAlive() const;

  void send(std::string const& msg);

private:
  void receivingService();

  ConnectedSocket socket_;

  MessageQueue& message_queue_;

  std::thread recv_thread_;
  std::atomic<bool> is_alive_ = false;
};
