#include <ClientApp/MessengerAppQt.h>
#include <ClientApp/UsersModelQt.h>

#include <QKeyEvent>
#include <QLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <iostream>

MessengerAppWidget::MessengerAppWidget(QWidget* parent) : QWidget(parent)
{
  this->setFixedSize(QSize(800, 600));

  // Create entrance widget.
  auto entrance_widget = new QWidget();

  auto username_label = new QLabel("Username: ");
  auto username_edit = new QLineEdit();
  auto confirm_button = new QPushButton("Log in!");

  auto content_layout = new QHBoxLayout();
  content_layout->addStretch();
  content_layout->addWidget(username_label);
  content_layout->addWidget(username_edit);
  content_layout->addWidget(confirm_button);
  content_layout->addStretch();

  auto vertical_layout = new QHBoxLayout();
  vertical_layout->addStretch();
  vertical_layout->addLayout(content_layout);
  vertical_layout->addStretch();

  entrance_widget->setLayout(vertical_layout);

  // Create main widget.
  auto main_widget = new QWidget();

  users_list_view_ = new QListView();
  users_model_ = new UsersModel(this);
  users_list_view_->setModel(users_model_);

  text_input_area_ = new QTextEdit();
  text_input_area_->installEventFilter(this);

  text_output_area_ = new QTextEdit();
  text_output_area_->setReadOnly(true);

  auto users_layout = new QVBoxLayout();
  users_layout->addWidget(users_list_view_);

  auto text_layout = new QVBoxLayout();
  text_layout->addWidget(text_output_area_, 2);
  text_layout->addWidget(text_input_area_, 1);

  auto main_layout = new QHBoxLayout();
  main_layout->addLayout(users_layout, 1);
  main_layout->addLayout(text_layout, 2);

  main_widget->setLayout(main_layout);

  // Create stacked widget.
  auto stacked_widget = new QStackedWidget();
  stacked_widget->addWidget(entrance_widget);
  stacked_widget->addWidget(main_widget);

  auto layout = new QHBoxLayout();
  layout->addWidget(stacked_widget);
  this->setLayout(layout);

  // Make connections.
  QObject::connect(confirm_button, &QPushButton::clicked, 
    [this, stacked_widget, main_widget, username_edit]() 
    {
      const auto username = username_edit->text();

      username_ = username;

      stacked_widget->setCurrentWidget(main_widget);

      emit userLoggedIn(username);
    });
}

void MessengerAppWidget::addUser(int id, QString const& name)
{
  users_model_->addUser(id, name);
}

void MessengerAppWidget::removeUser(int id)
{
  users_model_->removeUser(id);
}

void MessengerAppWidget::setUserMessage(int id, QString const& message)
{
  const auto name = users_model_->getUserName(id);
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
  auto const input_text = username_ + QString(": ") + text_input_area_->toPlainText();
  text_output_area_->append(input_text);
  text_input_area_->clear();

  const auto row = users_list_view_->currentIndex().row();
  const auto user = users_model_->getUserAt(row);

  emit sendMessageToUser(user.first, input_text);
}