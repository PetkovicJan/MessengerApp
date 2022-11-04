#pragma once

#include <QWidget>
#include <QListView>
#include <QTextEdit>

class MessengerAppWidget : public QWidget
{
  Q_OBJECT
public:
  explicit MessengerAppWidget(QWidget* parent = nullptr);

private:
  bool eventFilter(QObject* obj, QEvent* event) override;

  void onInputTextEnterPressed();

  QListView* users_list_view_ = nullptr;
  QTextEdit* text_input_area_ = nullptr;
};
