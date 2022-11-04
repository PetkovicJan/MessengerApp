#include "MessengerAppQt.h"

#include <QTextEdit>
#include <QLayout>

MessengerAppWidget::MessengerAppWidget(QWidget* parent) : QWidget(parent)
{
  this->setFixedSize(QSize(800, 600));

  // Create sub-widgets.
  users_list_view_ = new QListView();

  auto text_input_area = new QTextEdit();

  auto text_output_area = new QTextEdit();
  text_output_area->setReadOnly(true);

  // Set layout.
  auto users_layout = new QVBoxLayout();
  users_layout->addWidget(users_list_view_);

  auto text_layout = new QVBoxLayout();
  text_layout->addWidget(text_output_area, 2);
  text_layout->addWidget(text_input_area, 1);

  auto main_layout = new QHBoxLayout();
  main_layout->addLayout(users_layout, 1);
  main_layout->addLayout(text_layout, 2);

  this->setLayout(main_layout);
}
