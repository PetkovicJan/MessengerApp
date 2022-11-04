#include "MessengerAppQt.h"

#include <QApplication>
#include <QMainWindow>

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);

  // The deletion of this object is taken care by the Qt system.
  auto main_widget = new MessengerAppWidget();

  QMainWindow main_window;
  main_window.setCentralWidget(main_widget);
  main_window.move(50, 50);
  main_window.show();

  return a.exec();
}
