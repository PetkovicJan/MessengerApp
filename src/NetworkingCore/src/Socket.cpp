#include <NetworkingCore/Socket.h>

#include <stdexcept>
#include <cstdint>
#include <format>

SocketContext::SocketContext()
{
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    throw std::runtime_error("Startup failed.");
}

SocketContext::~SocketContext()
{
  WSACleanup();
}

Address::Address(SocketType type, const char* ip_str, const char* port_str)
{
  addrinfo host_info;
  ZeroMemory(&host_info, sizeof(host_info));
  host_info.ai_family = AF_INET;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_protocol = IPPROTO_TCP;
  if (type == SocketType::Server)
    host_info.ai_flags = AI_PASSIVE;

  if (getaddrinfo(ip_str, port_str, &host_info, &address_info_) != 0)
  {
    throw std::runtime_error("Get address info failed.");
  }
}

Address::~Address()
{
  if (address_info_)
    freeaddrinfo(address_info_);
}

int Address::family() const
{
  return address_info_->ai_family;
}

int Address::socket_type() const
{
  return address_info_->ai_socktype;
}

int Address::protocol() const
{
  return address_info_->ai_protocol;
}

BareSocket::BareSocket(int family, int socket_type, int protocol)
{
  socket_ = socket(family, socket_type, protocol);
  if (socket_ == INVALID_SOCKET)
  {
    throw std::runtime_error("Creating socket failed.");
  }
}

BareSocket::BareSocket(SOCKET socket)
{
  socket_ = socket;
}

BareSocket::~BareSocket()
{
  if(this->isValid())
    closesocket(socket_);

  socket_ = INVALID_SOCKET;
}

BareSocket::BareSocket(BareSocket&& other) noexcept
{
  socket_ = other.socket_;
  other.socket_ = INVALID_SOCKET;
}

SOCKET BareSocket::handle() const
{
  return socket_;
}

bool BareSocket::isValid() const
{
  return socket_ != INVALID_SOCKET;
}

std::string MessageBuilder::prefixWithLength(std::string const& msg)
{
  MsgSizeT const msgSize = msg.size();

  std::string prefixed;
  prefixed.resize(msgSize + SizeByteCount);

  std::memcpy(prefixed.data(), &msgSize, SizeByteCount);
  std::memcpy(prefixed.data() + SizeByteCount, msg.data(), msgSize);

  return prefixed;
}

std::vector<std::string> MessageBuilder::build(std::string const& currentMsg)
{
  auto currentMsgBegin = currentMsg.begin();

  if (!m_isMsgProcessed) {
    // Start processing new message. In current implementation we require 
    // that the obtained data contains at least the message size.
    if (currentMsg.size() < SizeByteCount) {
      throw std::runtime_error(std::format(
        "Received data for new message must be at least {} bytes long.", SizeByteCount));
    }

    // First bytes correspond to size of the message.
    std::memcpy(&m_processedMsgSz, currentMsg.data(), SizeByteCount);

    currentMsgBegin += 4;
    m_isMsgProcessed = true;
  }

  auto const currentMsgSize = currentMsg.end() - currentMsgBegin;
  auto const requiredSizeToComplete = m_processedMsgSz - m_processedMsg.size();
  if (requiredSizeToComplete > currentMsgSize) {
    // There is not enough data to complete the currently processed message. 
    // Just append the current data and return empty result.
    m_processedMsg.append(currentMsgBegin, currentMsg.end());

    return {};
  } else {
    // Complete currently processed message.
    const auto processedMsgEnd = currentMsgBegin + requiredSizeToComplete;

    auto completeMsg = m_processedMsg;
    completeMsg.append(currentMsgBegin, processedMsgEnd);

    // Reset the builder.
    m_isMsgProcessed = false;
    m_processedMsgSz = 0;
    m_processedMsg.clear();

    // If we used up all the data, then just return the result.
    if (processedMsgEnd == currentMsg.end()) {
      return { completeMsg };
    }
    
    // Recursively call this function on the remaining data.
    std::string remainingMsg(processedMsgEnd, currentMsg.end());
    auto remainingMsgs = build(remainingMsg);

    // Insert the first message at the begining to preserve the order.
    remainingMsgs.insert(remainingMsgs.begin(), completeMsg);

    return remainingMsgs;
  }
}

ConnectedSocket::ConnectedSocket(Address const& address) : 
  socket_(address.family(), address.socket_type(), address.protocol())
{
  auto const& ai = address.address_info_;
  if (connect(socket_.handle(), ai->ai_addr, ai->ai_addrlen) == SOCKET_ERROR)
  {
    throw std::runtime_error("Connecting to address failed.");
  }
}

ConnectedSocket::ConnectedSocket(SOCKET connected_socket) : 
  socket_(connected_socket)
{
}

ConnectedSocket::ConnectedSocket(ConnectedSocket&& other) noexcept : 
  socket_(std::move(other.socket_)) 
{
}

ConnectedSocket::~ConnectedSocket()
{
  shutdown(socket_.handle(), SD_SEND);
}

void ConnectedSocket::send(std::string const& data) const
{
  const auto prefixed = MessageBuilder::prefixWithLength(data);
  const int num_bytes = prefixed.size();
  const int result = ::send(socket_.handle(), prefixed.data(), num_bytes, 0);
  if (result == SOCKET_ERROR)
    throw std::runtime_error("Send failed.");
}

std::optional<std::vector<std::string>> ConnectedSocket::try_receive() const
{
  // Check the read status of the socket.
  fd_set read_set;
  FD_ZERO(&read_set);
  FD_SET(socket_.handle(), &read_set);

  timeval time_interval;
  time_interval.tv_sec = 1;

  // If not ready after timeout, return early.
  if (select(0, &read_set, NULL, NULL, &time_interval) == 0)
    return std::vector<std::string>();

  // The socket is ready, read out the data.
  const int buff_size = 512;
  std::string buffer('x', buff_size);
  const int num_bytes = buffer.size();

  int result = recv(socket_.handle(), &buffer.front(), num_bytes, 0);
  if (result > 0)
  {
    const int num_received = result;
    buffer.resize(num_received);

    const auto messages = builder_.build(buffer);
    return messages;
  }
  else
    return std::nullopt;
}

ListeningSocket::ListeningSocket(Address const& address, int max_connections) : 
  socket_(address.family(), address.socket_type(), address.protocol())
{
  auto const& ai = address.address_info_;
  if (bind(socket_.handle(), ai->ai_addr, ai->ai_addrlen) == SOCKET_ERROR)
  {
    throw std::runtime_error("Binding to address failed.");
  }

  if (listen(socket_.handle(), max_connections) == SOCKET_ERROR)
  {
    throw std::runtime_error("Listening failed.");
  }
}

ListeningSocket::~ListeningSocket()
{
}

ListeningSocket::ListeningSocket(ListeningSocket&& other) noexcept : 
  socket_(std::move(other.socket_))
{
}

ConnectedSocket ListeningSocket::accept()
{
  SOCKET client_socket = ::accept(socket_.handle(), NULL, NULL);
  if (client_socket == INVALID_SOCKET)
  {
    throw std::runtime_error("Accepting a client socket failed.");
  }

  return std::move(ConnectedSocket(client_socket));
}
