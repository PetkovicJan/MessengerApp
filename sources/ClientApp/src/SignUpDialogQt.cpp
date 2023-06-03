#include <ClientApp/SignUpDialogQt.h>

#include <QLabel>
#include <QLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

namespace detail
{
  static bool verify_password(std::string const& password)
  {
    constexpr int min_length = 5;

    const bool is_min_length = password.size() >= min_length;

    return is_min_length;
  }
}
SignUpDialogQt::SignUpDialogQt(QWidget* parent) : QDialog(parent)
{
  // Create elements.
  auto new_account_label = new QLabel("Create new account");

  auto username_label = new QLabel("Username: ");
  username_edit_ = new QLineEdit();

  auto password_label = new QLabel("Password: ");
  password_edit_ = new QLineEdit();
  password_edit_->setEchoMode(QLineEdit::Password);

  auto confirm_password_label = new QLabel("Confirm password: ");
  auto confirm_password_edit = new QLineEdit();
  confirm_password_edit->setEchoMode(QLineEdit::Password);

  auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  // Set layout.
  auto form_layout = new QGridLayout();
  form_layout->addWidget(username_label, 0, 0);
  form_layout->addWidget(username_edit_, 0, 1);
  form_layout->addWidget(password_label, 1, 0);
  form_layout->addWidget(password_edit_, 1, 1);
  form_layout->addWidget(confirm_password_label, 2, 0);
  form_layout->addWidget(confirm_password_edit, 2, 1);
  
  auto main_layout = new QVBoxLayout();
  main_layout->addWidget(new_account_label, 0, Qt::AlignCenter);
  main_layout->addLayout(form_layout);
  main_layout->addWidget(buttons, 0, Qt::AlignRight);
  this->setLayout(main_layout);

  // Make connections.
  QObject::connect(buttons, &QDialogButtonBox::accepted, 
    [this, confirm_password_edit]() 
    {
      // Verify that passwords are the same.
      const auto password = this->password_edit_->text();
      const auto confirm_password = confirm_password_edit->text();
      if (password != confirm_password)
      {
        QMessageBox msg;
        msg.setText("Password does not match confirmed password.");
        msg.exec();
        return;
      }

      if (!detail::verify_password(password.toStdString()))
      {
        QMessageBox msg;
        msg.setText("Password does not satisy the requirements.");
        msg.exec();
        return;
      }

      this->accept();
    });

  QObject::connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

std::pair<QString, QString> SignUpDialogQt::getUsernameAndPassword() const
{
  // Read username and password.
  const auto username = username_edit_->text();
  const auto password = password_edit_->text();

  return std::make_pair(username, password);
}
