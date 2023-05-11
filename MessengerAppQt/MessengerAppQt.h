#pragma once

#include <QWidget>
#include <QListView>
#include <QTextEdit>

class MessengerAppWidget : public QWidget
{
  Q_OBJECT
public:
  explicit MessengerAppWidget(QWidget* parent = nullptr);

  void addUser(int id, QString const& name);

  void removeUser(int id);

  void setUserMessage(int id, QString const& message);

signals:
  void sendMessageToUser(int id, QString const& message);

private:
  bool eventFilter(QObject* obj, QEvent* event) override;

  void enterInputMessage();

  QListView* users_list_view_ = nullptr;
  QTextEdit* text_input_area_ = nullptr;
  QTextEdit* text_output_area_ = nullptr;
};
