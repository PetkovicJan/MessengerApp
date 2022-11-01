#pragma once

#include "Socket.h"
#include "ThreadSafeQueue.h"
#include "Connection.h"

#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <optional>

class Server
{
public:
  explicit Server(
    std::string const& ip_str, std::string const& port_str, int max_num_clients);
  virtual ~Server();

  void exec();

protected:
  // Utility functions, that should be used by the derived classes.
  void stopServer();
  void sendMessageToClient(int client_id, std::string msg);
  void sendMessageToAllClients(
    std::string msg, std::optional<int> ignore_client_id = std::nullopt);

private:
  // Virtual functions, that should be overridden by derived classes.
  virtual void onClientConnected(int client_id);
  virtual void onClientDisconnected(int client_id);
  virtual void onClientMessageReceived(int client_id, std::string const& msg);

  void handleMessage(Message const& msg);

  void acceptingService();

  std::unique_ptr<Connection> acceptConnection();

  SocketContext context;

  std::atomic<bool> server_running_ = false;

  ListeningSocket listening_socket_;
  std::thread accepting_thread_;
  int id_counter_ = 0;

  // (Unique) pointer is mostly used to avoid moving non-copyable Connection 
  // instances. They contain a running thread, which makes moving impossible.
  std::vector<std::unique_ptr<Connection>> clients_;
  const int max_clients_ = 0;
  std::mutex clients_mutex_;

  MessageQueue message_queue_;
};
