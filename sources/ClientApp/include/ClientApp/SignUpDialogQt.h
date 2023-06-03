#include <QDialog>
#include <QLineEdit>

class SignUpDialogQt : public QDialog
{
  Q_OBJECT

public:
  explicit SignUpDialogQt(QWidget* parent = nullptr);

  std::pair<QString, QString> getUsernameAndPassword() const;

private:
  QLineEdit* username_edit_ = nullptr;
  QLineEdit* password_edit_ = nullptr;
};