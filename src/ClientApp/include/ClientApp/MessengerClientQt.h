#pragma once

#include <NetworkingCore/MessengerClient.h>

#include <QObject>

class MessengerClientQt : public QObject, public MessengerClient
{
  Q_OBJECT

public:
  explicit MessengerClientQt(
    const char* ip_str, const char* port_str, 
    QObject* parent = nullptr);

signals:
  void userLoggedIn(int user_id, QString const& name);
  void userLoggedOut(int user_id);
  void userMessageReceived(int user_id, QString const& msg);
  void userCreatedStatusReceived(CreateStatus status);
  void loginStatusReceived(LoginStatus status);

private:
  void onUserLoggedIn(int user_id, std::string const& username) override;
  void onUserLoggedOut(int user_id) override;
  void onUserMessageReceived(int user_id, std::string const& msg) override;
  void onUserCreatedStatusReceived(CreateStatus status) override;
  void onLoginStatusReceived(LoginStatus status) override;
};
