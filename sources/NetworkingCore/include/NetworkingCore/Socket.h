#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>
#include <vector>
#include <optional>

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
  explicit Address(SocketType type, const char* ip_str, const char* port_str);

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

// Class for message building using message length prefix.
class MessageBuilder
{
public:
  static std::string prefix_with_length(std::string const& data);

  // Build message from partial data. If any messages are completed with the input data,
  // they are returned as the result. If no messages are built, the vector is empty.
  std::vector<std::string> build(std::string const& data);

private:
  bool msg_started_ = false;
  int32_t msg_sz_ = 0;
  std::string msg_;
};

class ConnectedSocket
{
public:
  explicit ConnectedSocket(Address const& address);

  explicit ConnectedSocket(SOCKET connected_socket);

  ConnectedSocket(ConnectedSocket&& other) noexcept;

  ~ConnectedSocket();

  void send(std::string const& data) const;

  // Returns all the obtained messages. If no messages are obtained, the vector is empty.
  // If nullopt is returned, the connection is lost.
  std::optional<std::vector<std::string>> try_receive() const;

private:
  BareSocket socket_;

  mutable MessageBuilder builder_;
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
