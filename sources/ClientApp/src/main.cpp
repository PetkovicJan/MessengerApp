#include <QApplication>
#include <QMainWindow>

#include <ClientApp/MessengerAppQt.h>
#include <ClientApp/MessengerClientQt.h>

//#define DEBUG

int main(int argc, char* argv[]) {
  // This machine's IP.
  auto const ip_str = "127.0.0.1";

  // Port number from MS documentation.
  auto const port_num_str = "27015";

#ifndef DEBUG
  MessengerClientQt client(ip_str, port_num_str);
  std::thread client_thread([&client]() { client.exec(); });
#endif

  QApplication a(argc, argv);

  // The deletion of this object is taken care by the Qt system.
  auto main_widget = new MessengerAppWidget();
#ifdef DEBUG
  main_widget->addUser(0, "A");
  main_widget->addUser(1, "B");
  main_widget->addUser(2, "C");
#endif

#ifndef DEBUG

  // Handle events from the client side.
  QObject::connect(main_widget, &MessengerAppWidget::userLoggedIn, 
    [&client](QString const& username) 
    {
      AppMessage app_msg(AppMessageType::UserLoggedIn);
      app_msg << username.toStdString();

      client.sendMessage(app_msg);
    });

  QObject::connect(main_widget, &MessengerAppWidget::sendMessageToUser, 
    [&client](int id, QString const& msg)
    {
      AppMessage app_msg(AppMessageType::UserSentMessage);
      app_msg << id << msg.toStdString();

      client.sendMessage(app_msg);
    });

  // Handle events from the server side.
  QObject::connect(&client, &MessengerClientQt::userLoggedIn, main_widget,
                   &MessengerAppWidget::addUser);
  QObject::connect(&client, &MessengerClientQt::userLoggedOut, main_widget,
                   &MessengerAppWidget::removeUser);
  QObject::connect(&client, &MessengerClientQt::userMessageReceived,
                   main_widget, &MessengerAppWidget::setUserMessage);

#endif

  QMainWindow main_window;
  main_window.setCentralWidget(main_widget);
  main_window.move(200, 200);
  main_window.show();

  a.exec();

#ifndef DEBUG
  // Detach from server if app was exited.
  client.stopClient();
  if (client_thread.joinable()) client_thread.join();
#endif

  return 0;
}
