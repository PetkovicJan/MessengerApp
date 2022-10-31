#include "Connection.h"

Connection::Connection(ConnectedSocket&& socket, MessageQueue& message_queue) : 
  socket_(std::move(socket)), message_queue_(message_queue)
{
  is_alive_.store(true);
  recv_thread_ = std::move(std::thread(&Connection::receivingService, this));
}

Connection::~Connection()
{
  is_alive_.store(false);
  if (recv_thread_.joinable())
    recv_thread_.join();
}

bool Connection::isAlive() const
{
  return is_alive_.load();
}

void Connection::send(std::string const& msg)
{
  socket_.send(msg);
}

void Connection::receivingService()
{
  while (is_alive_.load())
  {
    auto const msg = socket_.receive();
    if (msg.size() > 0)
      message_queue_.push(msg);
    else
    {
      message_queue_.push("Client disconnected.");
      is_alive_.store(false);
    }
  }
}
