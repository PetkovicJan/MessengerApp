#pragma once

#include <ClientApp/UsersModelQt.h>

#include <QWidget>
#include <QListView>
#include <QTextEdit>
#include <QStringListModel>
#include <QStackedWidget>

#include <optional>

class MessengerAppWidget : public QWidget
{
  Q_OBJECT
public:
  explicit MessengerAppWidget(QWidget* parent = nullptr);

  void addUser(int id, QString const& name);
  void removeUser(int id);

  void setUserMessage(int id, QString const& message);

  void enterMainWidget();

  void displayInvalidUsernameMessage();
  void displayInvalidPasswordMessage();

  void displayUserCreatedSuccess();
  void displayUserCreatedFailure();

signals:
  void newUserCreated(QString const& username, QString const& password);
  void userLoggedIn(QString const& username, QString const& password);
  void sendMessageToUser(int id, QString const& message);

private:
  bool eventFilter(QObject* obj, QEvent* event) override;

  void enterInputMessage();

  std::optional<std::pair<int, QString>> getCurrentlySelectedUser() const;

  QString username_;

  QStackedWidget* stacked_widget_ = nullptr;

  QListView* users_list_view_ = nullptr;
  UsersModel* users_model_ = nullptr;

  // Dialog view and corresponding models representing dialog for each logged in user.
  QListView* dialog_view_ = nullptr;
  std::map<int, QStringListModel*> user_dialogs_;

  QTextEdit* text_input_area_ = nullptr;
};
