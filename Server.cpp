#include "Server.h"

#include <iostream>

Server::Server(std::string const& ip_str, std::string const& port_str, int max_num_clients) :
  listening_socket_(Address(SocketType::Server, ip_str, port_str), max_num_clients),
  max_clients_(max_num_clients)
{
}

Server::~Server()
{
  server_running_.store(false);

  if (accepting_thread_.joinable())
    accepting_thread_.join();
}

void Server::exec()
{
  server_running_.store(true);

  accepting_thread_ = std::move(std::thread(&Server::acceptingService, this));

  while (server_running_.load())
  {
    while (!message_queue_.empty())
    {
      auto msg = message_queue_.pop();
      onMessageReceived(msg);
    }
  }
}

void Server::stop()
{
  server_running_.store(false);
}

void Server::onMessageReceived(std::string const& msg)
{
  std::cout << msg << std::endl;
}

void Server::acceptingService()
{
  while (server_running_.load())
  {
    auto client_connection = acceptConnection();
    {
      std::lock_guard<std::mutex> lg(clients_mutex_);

      const int num_clients = clients_.size();
      if (num_clients < max_clients_)
      {
        client_connection->send("Connection with server succeeded.");
        clients_.push_back(std::move(client_connection));
        message_queue_.push("New client connected.");
      }
      else
      {
        client_connection->send("Connection with server failed.");
      }
    }
  }
}

std::unique_ptr<Connection> Server::acceptConnection()
{
  auto client_socket = listening_socket_.accept();

  return std::make_unique<Connection>(
    id_counter_++, std::move(client_socket), message_queue_);
}
