#include "Messenger.h"

#include <iostream>

MessengerApp::MessengerApp(ConnectedSocket& socket) : socket_(socket)
{
}

void MessengerApp::start()
{
  stop_flag_.store(false);

  auto const receiver_fun = [this]()
  {
    while (true)
    {
      auto const received_msg = socket_.receive();
      if (received_msg.size() == 0)
      {
        stop_flag_.store(true);
        return;
      }

      displayMessage(received_msg);
    }
  };

  receiver_ = std::move(std::thread(receiver_fun));

  while (!stop_flag_.load())
  {
    auto const msg = queryMessage();
    socket_.send(msg);
  }

  if (receiver_.joinable())
    receiver_.join();
}

std::vector<char> MessengerApp::queryMessage()
{
  std::string msg;
  std::cin >> msg;

  std::vector<char> result;
  for (int i = 0; i < msg.size(); ++i)
    result.push_back(msg[i]);

  return result;
}

void MessengerApp::displayMessage(std::vector<char> const& msg)
{
  std::string msg_str;
  for (auto ch : msg)
    msg_str.push_back(ch);

  std::lock_guard<std::mutex> lock(output_mtx_);
  std::cout << msg_str << '\n';
}
