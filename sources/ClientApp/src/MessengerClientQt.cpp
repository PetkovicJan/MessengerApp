#include <ClientApp/MessengerClientQt.h>

#include <iostream>

MessengerClientQt::MessengerClientQt(
  const char* ip_str, const char* port_str, QObject* parent) : 
  QObject(parent), MessengerClient(ip_str, port_str) 
{
}

void MessengerClientQt::onUserLoggedIn(int user_id, std::string const& name)
{
  std::cout << "Client::onUserLoggedIn: " << user_id << " " << name << '\n';
  emit userLoggedIn(user_id, QString::fromStdString(name));
}

void MessengerClientQt::onUserLoggedOut(int user_id)
{
  std::cout << "Client::onUserLoggedOut: " << user_id << '\n';
  emit userLoggedOut(user_id);
}

void MessengerClientQt::onUserMessageReceived(int user_id, std::string const& msg)
{
  std::cout << "Client::onUserMessageReceived: " << user_id << '\n';
  emit userMessageReceived(user_id, QString::fromStdString(msg));
}

void MessengerClientQt::onUserCreatedStatusReceived(CreateStatus status)
{
  std::cout << "Client::onUserCreatedStatusReceived: " << static_cast<int>(status) << '\n';
  emit userCreatedStatusReceived(status);
}

void MessengerClientQt::onLoginStatusReceived(LoginStatus status)
{
  std::cout << "Client::onLoginStatusReceived: " << static_cast<int>(status) << '\n';
  emit loginStatusReceived(status);
}
