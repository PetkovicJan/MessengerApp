#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>

enum class SocketType { Server, Client };

class SocketContext
{
public:
  explicit SocketContext();

  ~SocketContext();
};

class Address
{
public:
  explicit Address(SocketType type, std::string const& ip_str, std::string const& port_str);

  ~Address();

  int family() const;

  int socket_type() const;

  int protocol() const;

private:
  addrinfo* address_info_ = nullptr;

  friend class ConnectedSocket;
  friend class ListeningSocket;
};

class BareSocket
{
public:
  explicit BareSocket(int family, int socket_type, int protocol);

  explicit BareSocket(SOCKET socket);

  ~BareSocket();

  BareSocket(BareSocket const&) = delete;
  BareSocket& operator=(BareSocket const&) = delete;

  BareSocket(BareSocket&& other) noexcept;

  SOCKET handle() const;

  bool isValid() const;

private:
  SOCKET socket_ = INVALID_SOCKET;
};

class ConnectedSocket
{
public:
  explicit ConnectedSocket(Address const& address);

  explicit ConnectedSocket(SOCKET connected_socket);

  ConnectedSocket(ConnectedSocket&& other) noexcept;

  ~ConnectedSocket();

  void send(std::string const& data) const;

  std::string receive() const;

private:
  BareSocket socket_;
};

class ListeningSocket
{
public:
  explicit ListeningSocket(Address const& address, int max_connections);

  ~ListeningSocket();

  ListeningSocket(ListeningSocket&& other) noexcept;

  ConnectedSocket accept();

private:
  BareSocket socket_;
};