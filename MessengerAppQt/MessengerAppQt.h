#pragma once

#include <QWidget>
#include <QListView>

class MessengerAppWidget : public QWidget
{
  Q_OBJECT
public:
  explicit MessengerAppWidget(QWidget* parent = nullptr);

private:
  QListView* users_list_view_ = nullptr;
};
