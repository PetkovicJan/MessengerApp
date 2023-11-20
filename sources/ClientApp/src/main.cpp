#include <QApplication>
#include <QMainWindow>

#include <ClientApp/MessengerAppQt.h>
#include <ClientApp/MessengerClientQt.h>

#include <iostream>
#include <thread>

//#define DEBUG

#define LOOPBACK

int main(int argc, char* argv[]) {

  qRegisterMetaType<LoginStatus>("LoginStatus");
  qRegisterMetaType<CreateStatus>("ProcessHandle");

#ifdef LOOPBACK
  const char* ip_str = "127.0.0.1";
#else
  // Here goes the IP address of the server.
  const char* ip_str = "";
#endif

  // Port number from MS documentation.
  auto const port_num_str = "27015";

#ifndef DEBUG
  MessengerClientQt client(ip_str, port_num_str);
  std::thread client_thread([&client]() { 
    try
    {
      client.exec();
    }
    catch (std::exception const& e)
    {
      std::cout << "Exception was caught in client: " << e.what() << '\n';
    }
    });
#endif

  QApplication a(argc, argv);

  // The deletion of this object is taken care by the Qt system.
  auto main_widget = new MessengerAppWidget();
#ifdef DEBUG
  main_widget->addUser(0, "user1");
  main_widget->addUser(1, "user2");
  main_widget->addUser(2, "user3");
#endif

#ifdef DEBUG
  // In debug mode, enter main widget on login request without any authentication 
  // on the server side, since we are not connected to it at all.
  QObject::connect(main_widget, &MessengerAppWidget::userLoggedIn, 
    [main_widget](QString const& username, QString const& password) 
    {
      main_widget->enterMainWidget();
    });
#else
  // Handle events from the client side.
  QObject::connect(main_widget, &MessengerAppWidget::newUserCreated, 
    [&client](QString const& username, QString const& password) 
    {
      json login_msg;
      login_msg["type"] = AppMessageType::UserCreated;
      login_msg["username"] = username.toStdString();
      login_msg["password"] = password.toStdString();

      client.sendMessage(login_msg);
    });

  QObject::connect(main_widget, &MessengerAppWidget::userLoggedIn, 
    [&client](QString const& username, QString const& password) 
    {
      json login_msg;
      login_msg["type"] = AppMessageType::UserLoggedIn;
      login_msg["username"] = username.toStdString();
      login_msg["password"] = password.toStdString();

      client.sendMessage(login_msg);
    });

  QObject::connect(main_widget, &MessengerAppWidget::sendMessageToUser, 
    [&client](int id, QString const& msg)
    {
      json user_msg;
      user_msg["type"] = AppMessageType::UserSentMessage;
      user_msg["user_id"] = id;
      user_msg["message"] = msg.toStdString();

      client.sendMessage(user_msg);
    });

  // Handle events from the server side.
  QObject::connect(&client, &MessengerClientQt::userLoggedIn, main_widget,
                   &MessengerAppWidget::addUser);
  QObject::connect(&client, &MessengerClientQt::userLoggedOut, main_widget,
                   &MessengerAppWidget::removeUser);
  QObject::connect(&client, &MessengerClientQt::userMessageReceived,
                   main_widget, &MessengerAppWidget::setUserMessage);
  QObject::connect(&client, &MessengerClientQt::loginStatusReceived, main_widget,
    [main_widget](LoginStatus status)
    {
      if (status == LoginStatus::Success)
        main_widget->enterMainWidget();
      else if (status == LoginStatus::InvalidUsername)
        main_widget->displayInvalidUsernameMessage();
      else if (status == LoginStatus::InvalidPassword)
        main_widget->displayInvalidPasswordMessage();
    });
  QObject::connect(&client, &MessengerClientQt::userCreatedStatusReceived, main_widget,
    [main_widget](CreateStatus status)
    {
      if (status == CreateStatus::Success)
        main_widget->displayUserCreatedSuccess();
      else if (status == CreateStatus::Failure)
        main_widget->displayUserCreatedFailure();
    });
#endif

  QMainWindow main_window;
  main_window.setCentralWidget(main_widget);
  main_window.move(200, 200);
  main_window.show();

  try
  {
    a.exec();
  }
  catch (std::exception const& e)
  {
    std::cout << "Exception was caught in GUI application: " << e.what() << '\n';
  }

#ifndef DEBUG
  // Detach from server if app was exited.
  client.stopClient();
  if (client_thread.joinable()) client_thread.join();
#endif

  return 0;
}
