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

std::string MessengerApp::queryMessage()
{
  std::string msg;
  std::cin >> msg;

  return msg;
}

void MessengerApp::displayMessage(std::string const& msg)
{
  std::lock_guard<std::mutex> lock(output_mtx_);
  std::cout << msg << '\n';
}

AppMessage::AppMessage(AppMessageType type, std::optional<std::string> const& opt_data)
{
  header_.type = type;

  if (opt_data)
    data_ = *opt_data;

  header_.size = sizeof(MessageHeader) + data_.size();
}

AppMessageType AppMessage::type() const
{
  return header_.type;
}

std::string AppMessage::data() const
{
  return data_;
}

std::string serialize(AppMessage const& msg)
{
  auto const header_sz = sizeof(AppMessage::MessageHeader);

  auto const& header = msg.header_;
  const auto& data = msg.data_;

  // Create an array of an appropriate size.
  std::string res;
  res.resize(header.size, 'x');

  // First copy the header part.
  std::memcpy(res.data(), &header, header_sz);

  // Then copy the data part.
  std::memcpy(res.data() + header_sz, data.data(), data.size());

  return res;
}

AppMessage deserialize(std::string const& str)
{
  auto const header_sz = sizeof(AppMessage::MessageHeader);

  // First read the header part.
  AppMessage::MessageHeader header;
  std::memcpy(&header, str.data(), header_sz);

  // Now read the data, since we know the size of the message.
  std::string data;
  auto const data_sz = header.size - header_sz;
  if (data_sz > 0)
  {
    data.resize(data_sz, 'x');
    std::memcpy(data.data(), str.data() + header_sz, data_sz);
  }

  return AppMessage(header.type, data);
}

template<>
AppMessage& operator << (AppMessage& msg, std::string const& put_data)
{
  msg.data_.append(put_data);

  return msg;
}

template<>
AppMessage& operator >> (AppMessage& msg, std::string& take_data)
{
  take_data = std::move(msg.data_);

  return msg;
}
