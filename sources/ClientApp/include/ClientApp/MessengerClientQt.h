#pragma once

#include <NetworkingCore/Messenger.h>

#include <QObject>

class MessengerClientQt : public QObject, public MessengerClient
{
  Q_OBJECT

public:
  explicit MessengerClientQt(
    std::string const& ip_str, std::string const& port_str, 
    QObject* parent = nullptr);

  void sendMessageToUser(int id, QString const& msg);

signals:
  void userLoggedIn(int user_id, QString const& name);
  void userLoggedOut(int user_id);
  void userMessageReceived(int user_id, QString const& msg);

private:
  void onUserLoggedIn(int user_id, std::string const& name) override;
  void onUserLoggedOut(int user_id) override;
  void onUserMessageReceived(int user_id, std::string const& msg) override;
};
