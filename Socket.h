#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>
#include <vector>

class SocketContext
{
public:
  explicit SocketContext();

  ~SocketContext();
};

class Address
{
public:
  explicit Address(std::string const& ip_str, std::string const& port_str);

  ~Address();

  int family() const;

  int socket_type() const;

  int protocol() const;

private:
  addrinfo* address_info_ = nullptr;

  friend class ConnectedSocket;
};

class BareSocket
{
public:
  explicit BareSocket(int family, int socket_type, int protocol);

  ~BareSocket();

  SOCKET handle() const;

  bool isValid() const;

private:
  SOCKET socket_ = INVALID_SOCKET;
};

class ConnectedSocket
{
public:
  explicit ConnectedSocket(Address const& address);

  ~ConnectedSocket();

  void send(std::vector<char> const& data);

  std::vector<char> receive();

private:
  BareSocket socket_;
};
