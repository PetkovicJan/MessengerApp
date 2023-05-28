#pragma once

#include <ClientApp/UsersModelQt.h>

#include <QWidget>
#include <QListView>
#include <QTextEdit>
#include <QStringListModel>

class MessengerAppWidget : public QWidget
{
  Q_OBJECT
public:
  explicit MessengerAppWidget(QWidget* parent = nullptr);

  void addUser(int id, QString const& name);

  void removeUser(int id);

  void setUserMessage(int id, QString const& message);

signals:
  void userLoggedIn(QString const& username);

  void sendMessageToUser(int id, QString const& message);

private:
  bool eventFilter(QObject* obj, QEvent* event) override;

  void enterInputMessage();

  std::optional<std::pair<int, QString>> getCurrentlySelectedUser() const;

  QString username_;

  QListView* users_list_view_ = nullptr;
  UsersModel* users_model_ = nullptr;

  // Dialog view and corresponding models representing dialog for each logged in user.
  QListView* dialog_view_ = nullptr;
  std::map<int, QStringListModel*> user_dialogs_;

  QTextEdit* text_input_area_ = nullptr;
};
