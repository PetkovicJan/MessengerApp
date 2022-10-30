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
};