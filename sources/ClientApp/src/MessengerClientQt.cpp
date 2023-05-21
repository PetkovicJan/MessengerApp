#include <ClientApp/MessengerClientQt.h>

MessengerClientQt::MessengerClientQt(
  std::string const& ip_str, std::string const& port_str, QObject* parent) : 
  QObject(parent), MessengerClient(ip_str, port_str) 
{
}

void MessengerClientQt::onUserLoggedIn(int user_id, std::string const& name)
{
  emit userLoggedIn(user_id, QString::fromStdString(name));
}

void MessengerClientQt::onUserLoggedOut(int user_id)
{
  emit userLoggedOut(user_id);
}

void MessengerClientQt::onUserMessageReceived(int user_id, std::string const& msg)
{
  emit userMessageReceived(user_id, QString::fromStdString(msg));
}
