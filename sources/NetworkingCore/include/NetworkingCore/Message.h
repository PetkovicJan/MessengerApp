#pragma once

#include <string>

enum class MessageType { ClientMessage, ClientConnected, ClientDisconnected };

struct Message
{
  MessageType type;
  int client_id;
  std::string msg;
};
