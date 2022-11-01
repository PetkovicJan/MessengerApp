#include "Connection.h"

Connection::Connection(
  int unique_id, ConnectedSocket&& socket, MessageQueue& message_queue) : 
  unique_id_(unique_id), socket_(std::move(socket)), message_queue_(message_queue)
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

// Note that sending and receiving from the same socket is thread safe. 
// What is not thread-safe is calling send from multiple threads (and the 
// same for receive).
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
      message_queue_.push(std::to_string(unique_id_) + msg);
    else
    {
      message_queue_.push(std::to_string(unique_id_) + "Client disconnected.");
      is_alive_.store(false);
    }
  }
}
