#include <ClientApp/MessengerAppQt.h>
#include <ClientApp/UsersModelQt.h>
#include <ClientApp/SignUpDialogQt.h>

#include <QKeyEvent>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include <iostream>

namespace detail
{
  static void appendStringToStringListModel(QStringListModel* model, QString const& string)
  {
    // Append row at the end.
    const auto row = model->rowCount();
    if (!model->insertRow(row))
      return;

    model->setData(model->index(row), string);
  }
}

MessengerAppWidget::MessengerAppWidget(QWidget* parent) : QWidget(parent)
{
  this->setFixedSize(QSize(800, 600));

  // Create entrance widget.
  auto entrance_widget = new QWidget();

  auto username_label = new QLabel("Username: ");
  auto username_edit = new QLineEdit();
  auto password_label = new QLabel("Password: ");
  auto password_edit = new QLineEdit();
  password_edit->setEchoMode(QLineEdit::Password);
  auto confirm_button = new QPushButton("Log in");

  auto sign_up_label = new QLabel("Don't have an account?");
  auto sign_up_button = new QPushButton("Sign up");

  auto log_in_layout = new QGridLayout();
  log_in_layout->addWidget(username_label, 0, 0);
  log_in_layout->addWidget(username_edit, 0, 1);
  log_in_layout->addWidget(password_label, 1, 0);
  log_in_layout->addWidget(password_edit, 1, 1);
  log_in_layout->addWidget(confirm_button, 2, 1, Qt::AlignRight);

  auto sign_up_layout = new QHBoxLayout();
  sign_up_layout->addWidget(sign_up_label);
  sign_up_layout->addWidget(sign_up_button);

  auto content_layout = new QVBoxLayout();
  content_layout->addStretch();
  content_layout->addLayout(log_in_layout);
  content_layout->addStretch();
  content_layout->addLayout(sign_up_layout);
  content_layout->addStretch();

  auto center_layout = new QHBoxLayout();
  center_layout->addStretch();
  center_layout->addLayout(content_layout);
  center_layout->addStretch();

  entrance_widget->setLayout(center_layout);

  // Create main widget.
  auto main_widget = new QWidget();

  users_list_view_ = new QListView();
  users_model_ = new UsersModel(this);
  users_list_view_->setModel(users_model_);

  text_input_area_ = new QTextEdit();
  text_input_area_->installEventFilter(this);

  dialog_view_ = new QListView();

  auto users_layout = new QVBoxLayout();
  users_layout->addWidget(users_list_view_);

  auto text_layout = new QVBoxLayout();
  text_layout->addWidget(dialog_view_, 2);
  text_layout->addWidget(text_input_area_, 1);

  auto main_layout = new QHBoxLayout();
  main_layout->addLayout(users_layout, 1);
  main_layout->addLayout(text_layout, 2);

  main_widget->setLayout(main_layout);

  // Create stacked widget.
  stacked_widget_ = new QStackedWidget();
  stacked_widget_->addWidget(entrance_widget);
  stacked_widget_->addWidget(main_widget);

  auto layout = new QHBoxLayout();
  layout->addWidget(stacked_widget_);
  this->setLayout(layout);

  // Make connections.

  // On login button clicked.
  QObject::connect(confirm_button, &QPushButton::clicked, 
    [this, username_edit, password_edit]() 
    {
      // Read username and password.
      const auto username = username_edit->text();
      const auto password = password_edit->text();
      username_ = username;

      // Notify the world about the login.
      emit userLoggedIn(username, password);
    });

  // On sign up button clicked.
  QObject::connect(sign_up_button, &QPushButton::clicked, 
    [this]() 
    {
      SignUpDialogQt sign_up_dialog(this);
      if (sign_up_dialog.exec() == QDialog::Accepted)
      {
        // Obtain username and password.
        const auto [username, password] = sign_up_dialog.getUsernameAndPassword();
        
        // Notify the world about the new account.
        emit newUserCreated(username, password);
      }
    });

  // On user selected (from users list).
  const auto selection_model = users_list_view_->selectionModel();
  QObject::connect(selection_model, &QItemSelectionModel::currentChanged, 
    [this](QModelIndex const& current_index) 
    {
      // If index invalid.
      if (current_index == QModelIndex())
      {
        dialog_view_->setModel(nullptr);
      }
      else
      {
        const auto [id, name] = users_model_->getUserAt(current_index.row());
        dialog_view_->setModel(user_dialogs_.at(id));
      }
    });
}

void MessengerAppWidget::addUser(int id, QString const& name)
{
  // First create a new dialog model.
  user_dialogs_.emplace(id, new QStringListModel(this));

  // Then add the user to the list.
  users_model_->addUser(id, name);
}

void MessengerAppWidget::removeUser(int id)
{
  // Remove the dialog model.

  // Sanity check.
  if (user_dialogs_.count(id))
  {
    // If the currently selected user needs to be removed, unset the dialog first.
    if (const auto opt_user = getCurrentlySelectedUser())
    {
      const auto current_id = opt_user->first;
      if (current_id == id) dialog_view_->setModel(nullptr);
    }

    user_dialogs_.erase(id);
  }

  // Remove the user from the users list.
  users_model_->removeUser(id);
}

void MessengerAppWidget::setUserMessage(int id, QString const& message)
{
  const auto name = users_model_->getUserName(id);
  const auto text = name + QString(": ") + message;

  const auto user_dialog = user_dialogs_.at(id);
  detail::appendStringToStringListModel(user_dialog, text);
}

void MessengerAppWidget::enterMainWidget()
{
  stacked_widget_->setCurrentIndex(1);
}

void MessengerAppWidget::displayInvalidUsernameMessage()
{
  QMessageBox msg;
  msg.setText("Entered username is not registered.");
  msg.exec();
}

void MessengerAppWidget::displayInvalidPasswordMessage()
{
  QMessageBox msg;
  msg.setText("Entered password is not valid.");
  msg.exec();
}

void MessengerAppWidget::displayUserCreatedSuccess()
{
  QMessageBox msg;
  msg.setText("New user created successfully.");
  msg.exec();
}

void MessengerAppWidget::displayUserCreatedFailure()
{
  QMessageBox msg;
  msg.setText("Failed to create new user.");
  msg.exec();
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
  // Obtain the currently selected user.
  const auto opt_user = getCurrentlySelectedUser();
  if (!opt_user) return;

  const auto id = opt_user->first;

  // Obtain the dialog for the currently selected user.
  const auto user_dialog = user_dialogs_.at(id);

  // First take the text from input area.
  const auto input_text = text_input_area_->toPlainText();
  text_input_area_->clear();

  // Add this user's text to the dialog.
  const auto text = username_ + QString(": ") + input_text;
  detail::appendStringToStringListModel(user_dialog, text);

  // Notfiy the world about newly added message.
  emit sendMessageToUser(id, input_text);
}

std::optional<std::pair<int, QString>> MessengerAppWidget::getCurrentlySelectedUser() const
{
  const auto idx = users_list_view_->selectionModel()->currentIndex();
  // Check if no user selected (index invalid).
  if (idx == QModelIndex())
    return std::nullopt;

  return users_model_->getUserAt(idx.row());
}
