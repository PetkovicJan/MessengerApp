#pragma once

#include "Socket.h"
#include "ThreadSafeQueue.h"

#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

class Server
{
public:
  explicit Server(std::string const& ip_str, std::string const& port_str, int max_num_clients);
  virtual ~Server();

  void exec();

protected:
  void stop();

  virtual void onMessageReceived(std::string const& msg);

private:
  void acceptingService();

  SocketContext context;

  std::atomic<bool> server_running_ = false;

  ListeningSocket listening_socket_;
  std::thread accepting_thread_;

  std::vector<ConnectedSocket> clients_;
  const int max_clients_ = 0;
  std::mutex clients_mutex_;

  ThreadSafeQueue<std::string> message_queue_;
};
