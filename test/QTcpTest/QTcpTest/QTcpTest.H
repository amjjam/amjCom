#ifndef QTCPTEST_H
#define QTCPTEST_H

#include <QMainWindow>
#include <QThread>

#include <amjCom.H>
#include <amjComTCP.H>

QT_BEGIN_NAMESPACE
namespace Ui { class QTcpTest; }
QT_END_NAMESPACE

class QTcpTest : public QMainWindow
{
  Q_OBJECT

public:
  QTcpTest(QWidget *parent = nullptr);
  ~QTcpTest();
signals:
  void receive_(QString s);
  void status_(QString s);
private:
  Ui::QTcpTest *ui;
  std::shared_ptr<amjCom::TCP::Client> client;
  void receive(amjCom::Packet &);
  void status(amjCom::Status);
  void startstop();
};
#endif // QTCPTEST_H
