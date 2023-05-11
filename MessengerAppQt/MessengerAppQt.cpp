#include "MessengerAppQt.h"

#include "UsersModelQt.h"

#include <QKeyEvent>
#include <QLayout>

#include <iostream>

MessengerAppWidget::MessengerAppWidget(QWidget* parent) : QWidget(parent)
{
  this->setFixedSize(QSize(800, 600));

  // Create sub-widgets.
  users_list_view_ = new QListView();
  auto users_model = new UsersModel(this);
  users_list_view_->setModel(users_model);

  text_input_area_ = new QTextEdit();
  text_input_area_->installEventFilter(this);

  text_output_area_ = new QTextEdit();
  text_output_area_->setReadOnly(true);

  // Set layout.
  auto users_layout = new QVBoxLayout();
  users_layout->addWidget(users_list_view_);

  auto text_layout = new QVBoxLayout();
  text_layout->addWidget(text_output_area_, 2);
  text_layout->addWidget(text_input_area_, 1);

  auto main_layout = new QHBoxLayout();
  main_layout->addLayout(users_layout, 1);
  main_layout->addLayout(text_layout, 2);

  this->setLayout(main_layout);
}

void MessengerAppWidget::addUser(int id, QString const& name)
{
  static_cast<UsersModel*>(users_list_view_->model())->addUser(id, name);
}

void MessengerAppWidget::removeUser(int id)
{
  static_cast<UsersModel*>(users_list_view_->model())->removeUser(id);
}

void MessengerAppWidget::setUserMessage(int id, QString const& message)
{
  const auto name = static_cast<UsersModel*>(users_list_view_->model())->getUserName(id);
  auto const text = name + QString(": ") + message;
  text_output_area_->append(text);
}

bool MessengerAppWidget::eventFilter(QObject* obj, QEvent* event)
{
  if (obj != text_input_area_)
    return QWidget::eventFilter(obj, event);

  if (event->type() != QEvent::KeyPress)
    return false;

  auto key_event = static_cast<QKeyEvent*>(event);
  if (key_event->key() == Qt::Key_Return)
  {
    enterInputMessage();
    return true;
  }

  return false;
}

void MessengerAppWidget::enterInputMessage()
{
  auto const input_text = QString("me: ") + text_input_area_->toPlainText();
  text_output_area_->append(input_text);
  text_input_area_->clear();

  const auto row = users_list_view_->currentIndex().row();
  const auto user = static_cast<UsersModel*>(users_list_view_->model())->getUserAt(row);

  emit sendMessageToUser(user.first, input_text);
}
