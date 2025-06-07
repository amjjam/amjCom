#include "QTcpTest.H"
#include "ui_QTcpTest.h"

#include <iostream>

QTcpTest::QTcpTest(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::QTcpTest),client(nullptr){
  ui->setupUi(this);
  connect(ui->pushButton_startstop,&QPushButton::clicked,this,&QTcpTest::startstop);
  connect(this,&QTcpTest::receive_,ui->textEdit_messages,&QTextEdit::append);
  connect(this,&QTcpTest::status_,ui->textEdit_status,&QTextEdit::append);
}

QTcpTest::~QTcpTest(){
  delete ui;
}

void QTcpTest::receive(amjCom::Packet &p){
  std::cout << "QTcpTest::receive" << std::endl;
  std::string s;
  s.resize(p.size());
  p.begin();
  memcpy(&s[0],p.read(p.size()),p.size());
  emit receive_(QString::fromStdString(s));
};

void QTcpTest::status(amjCom::Status s){
  emit status_(QString::fromStdString(s.statedescription()+": "+s.errormessage()));
};

void QTcpTest::startstop(){
  if(!client){
    std::string server=ui->lineEdit_serverDescription->text().toStdString();

    client=std::make_shared<amjCom::TCP::Client>(server,
                                     [this](amjCom::Packet &p){receive(p);},
                                     [this](amjCom::Status s){status(s);});
    client->start();
    ui->pushButton_startstop->setText("Stop");
  }
  else{
    client.reset();
    ui->pushButton_startstop->setText("Start");
  }
}
