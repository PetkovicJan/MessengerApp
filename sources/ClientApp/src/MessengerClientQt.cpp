#include <ClientApp/MessengerClientQt.h>

#include <iostream>

MessengerClientQt::MessengerClientQt(
  std::string const& ip_str, std::string const& port_str, QObject* parent) : 
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
