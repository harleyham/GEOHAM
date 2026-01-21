#include "widget.h"
#include <QtCore>
#include <QtWidgets>
#include <stdio.h>
#include <iostream>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    redirector = new StdoutRedirector(this, StdoutRedirector::StandardOutput|StdoutRedirector::StandardError);
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    QHBoxLayout *hbox = new QHBoxLayout(this);
    edit = new QPlainTextEdit(this);
    hbox->addWidget(edit);

    timer->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(redirector, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout2()));

    edit->appendPlainText("Teste\n");

}

Widget::~Widget()
{
    
}

void Widget::onTimeout()
{
    printf("From printf ... ");
    fflush(stdout);
    fprintf(stderr, "From fprintf...");
    std::cout<<"From std::cout..."<<std::endl;
    std::cerr<<"From std::cerr..."<<std::endl;
    qDebug()<<"From qDebug ..."<<QDateTime::currentDateTime().toString();
}

void Widget::readData()
{
    QString Text = QString::fromLocal8Bit(redirector->read(1024));
    edit->appendPlainText(Text);
}
