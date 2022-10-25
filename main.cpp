#include <iostream>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <vector>

class SocketContext
{
public:
  explicit SocketContext()
  {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
      throw std::runtime_error("Startup failed.");
  }

  ~SocketContext()
  {
    WSACleanup();
  }
};

class Address
{
public:
  explicit Address(std::string const& ip_str, std::string const& port_str)
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

  ~Address()
  {
    if (address_info_)
      freeaddrinfo(address_info_);
  }

  int family() const
  {
    return address_info_->ai_family;
  }

  int socket_type() const
  {
    return address_info_->ai_socktype;
  }

  int protocol() const
  {
    return address_info_->ai_protocol;
  }

private:
  addrinfo* address_info_ = nullptr;

  friend class ConnectedSocket;
};

class BareSocket
{
public:
  explicit BareSocket(int family, int socket_type, int protocol)
  {
    SOCKET socket_ = socket(family, socket_type, protocol);
    if (socket_ == INVALID_SOCKET)
    {
      throw std::runtime_error("Creating socket failed.");
    }
  }

  ~BareSocket()
  {
    if(this->isValid())
      closesocket(socket_);

    socket_ = INVALID_SOCKET;
  }

  SOCKET handle() const
  {
    return socket_;
  }

  bool isValid() const
  {
    return socket_ != INVALID_SOCKET;
  }

private:
  SOCKET socket_ = INVALID_SOCKET;
};

class ConnectedSocket
{
public:
  explicit ConnectedSocket(Address const& address) : 
    socket_(address.family(), address.socket_type(), address.protocol())
  {
    auto const& ai = address.address_info_;
    if (connect(socket_.handle(), ai->ai_addr, ai->ai_addrlen) == SOCKET_ERROR)
    {
      throw std::runtime_error("Connecting to address failed.");
    }
  }

  ~ConnectedSocket()
  {
    shutdown(socket_.handle(), SD_SEND);
  }

  void send(std::vector<char> const& data)
  {
    const int num_bytes = data.size();
    const int result = ::send(socket_.handle(), data.data(), num_bytes, 0);
    if (result == SOCKET_ERROR)
      throw std::runtime_error("Send failed.");
  }

  std::vector<char> receive()
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

private:
  BareSocket socket_;
};

int main(int argc, char* argv[])
{
  SocketContext socket_context;

  // This machine's IP.
  auto const ip_str = "192.168.8.1";

  // Port number from MS documentation.
  auto const port_num_str = "27015";
  Address server_address(ip_str, port_num_str);

  ConnectedSocket socket(server_address);

  socket.send({ 'H', 'i', '!' });

  auto const received_data = socket.receive();

  return 0;
}