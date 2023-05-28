#include <ClientApp/MessengerAppQt.h>
#include <ClientApp/UsersModelQt.h>

#include <QKeyEvent>
#include <QLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

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
  auto stacked_widget = new QStackedWidget();
  stacked_widget->addWidget(entrance_widget);
  stacked_widget->addWidget(main_widget);

  auto layout = new QHBoxLayout();
  layout->addWidget(stacked_widget);
  this->setLayout(layout);

  // Make connections.

  // On login button clicked.
  QObject::connect(confirm_button, &QPushButton::clicked, 
    [this, stacked_widget, main_widget, username_edit]() 
    {
      // Set the username.
      const auto username = username_edit->text();
      username_ = username;

      // Switch from login widget to the main widget.
      stacked_widget->setCurrentWidget(main_widget);

      // Notify the world about the login.
      emit userLoggedIn(username);
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
