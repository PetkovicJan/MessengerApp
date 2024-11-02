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

/** Class for preparing (building) message to be send (received) through wire. */
class MessageBuilder
{
public:
  /** Each message sent through the wire shall be prefixed with its length using this function. */
  static std::string prefixWithLength(std::string const& data);

  /**
   * Build message from data obtained from wire. The function returns the messages that have been
   * fully reconstructed with the @param data. Any partially constructed data shall be reconstructed
   * in subsequent calls to this function with appropriate data.
   */
  std::vector<std::string> build(std::string const& data);

private:
  using MsgSizeT = std::uint32_t;

  static constexpr std::size_t SizeByteCount = sizeof(MsgSizeT);

  /** Whether message build is in progress. */
  bool m_isMsgProcessed = false;

  /** Final size of the currently processed message. */
  MsgSizeT m_processedMsgSz = 0;

  /** Currently processed message. */
  std::string m_processedMsg;
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
