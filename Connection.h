#pragma once

#include "Socket.h"
#include "ThreadSafeQueue.h"

#include <thread>
#include <atomic>

class Connection
{
public:
  Connection(int unique_id, ConnectedSocket&& socket, MessageQueue& message_queue);
  ~Connection();

  int id() const;

  bool isAlive() const;

  void send(std::string const& msg);

private:
  void receivingService();

  const int unique_id_ = 0;
  ConnectedSocket socket_;

  MessageQueue& message_queue_;

  std::thread recv_thread_;
  std::atomic<bool> is_alive_ = false;
};
