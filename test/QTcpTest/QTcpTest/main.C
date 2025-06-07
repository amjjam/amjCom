#include "QTcpTest.H"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QTcpTest w;
  w.show();
  return a.exec();
}
