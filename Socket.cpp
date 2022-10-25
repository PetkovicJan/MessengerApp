#include "Socket.h"

#include <stdexcept>

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

Address::Address(std::string const& ip_str, std::string const& port_str)
{
  addrinfo host_info;
  ZeroMemory(&host_info, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_protocol = IPPROTO_TCP;

  if (getaddrinfo(ip_str.c_str(), port_str.c_str(), &host_info, &address_info_) != 0)
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
  SOCKET socket_ = socket(family, socket_type, protocol);
  if (socket_ == INVALID_SOCKET)
  {
    throw std::runtime_error("Creating socket failed.");
  }
}

BareSocket::~BareSocket()
{
  if(this->isValid())
    closesocket(socket_);

  socket_ = INVALID_SOCKET;
}

SOCKET BareSocket::handle() const
{
  return socket_;
}

bool BareSocket::isValid() const
{
  return socket_ != INVALID_SOCKET;
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

ConnectedSocket::~ConnectedSocket()
{
  shutdown(socket_.handle(), SD_SEND);
}

void ConnectedSocket::send(std::vector<char> const& data)
{
  const int num_bytes = data.size();
  const int result = ::send(socket_.handle(), data.data(), num_bytes, 0);
  if (result == SOCKET_ERROR)
    throw std::runtime_error("Send failed.");
}

std::vector<char> ConnectedSocket::receive()
{
  const int buff_size = 512;
  std::vector<char> buffer(buff_size);
  const int num_bytes = buffer.size();

  int result = recv(socket_.handle(), buffer.data(), num_bytes, 0);
  if (result > 0)
  {
    const int num_received = result;
    buffer.resize(num_received);

    return buffer;
  }
}
