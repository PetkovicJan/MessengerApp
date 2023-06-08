#include <NetworkingCore/Client.h>

#include <iostream>

Client::Client(std::string const& ip_str, std::string const& port_str)
{
  server_ = std::make_unique<ConnectedSocket>(
    Address(SocketType::Client, ip_str, port_str));
}

Client::~Client()
{
}

void Client::exec()
{
  is_alive_.store(true);
  while (is_alive_.load())
  {
    auto const messages = server_->try_receive();

    // Connection is lost.
    if (!messages.has_value()) break;

    if (messages->size() == 0) continue;

    for (const auto msg : *messages)
      onServerMessageReceived(msg);
  }

  onDisconnectedFromServer();
}

void Client::stopClient()
{
  is_alive_.store(false);
}

void Client::sendMessageToServer(std::string const& msg)
{
  server_->send(msg);
}

void Client::onServerMessageReceived(std::string const& msg)
{
  std::cout << "Server: " << msg << std::endl;
}

void Client::onDisconnectedFromServer()
{
  std::cout << "Disconnected from server." << std::endl;
}
