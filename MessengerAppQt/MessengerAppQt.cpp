#include "MessengerAppQt.h"

#include <QKeyEvent>
#include <QLayout>

#include <iostream>

MessengerAppWidget::MessengerAppWidget(QWidget* parent) : QWidget(parent)
{
  this->setFixedSize(QSize(800, 600));

  // Create sub-widgets.
  users_list_view_ = new QListView();

  text_input_area_ = new QTextEdit();
  text_input_area_->installEventFilter(this);

  auto text_output_area = new QTextEdit();
  text_output_area->setReadOnly(true);

  // Set layout.
  auto users_layout = new QVBoxLayout();
  users_layout->addWidget(users_list_view_);

  auto text_layout = new QVBoxLayout();
  text_layout->addWidget(text_output_area, 2);
  text_layout->addWidget(text_input_area_, 1);

  auto main_layout = new QHBoxLayout();
  main_layout->addLayout(users_layout, 1);
  main_layout->addLayout(text_layout, 2);

  this->setLayout(main_layout);
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
    onInputTextEnterPressed();
    return true;
  }

  return false;
}

void MessengerAppWidget::onInputTextEnterPressed()
{
  std::cout << "Enter pressed in input text area." << std::endl;
}
