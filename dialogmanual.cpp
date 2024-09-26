#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#include "dialogmanual.h"
#include "ui_dialogmanual.h"

DialogManual::DialogManual(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogManual)
{
    ui->setupUi(this);
}

DialogManual::~DialogManual()
{
    delete ui;
}

void DialogManual::print(int size,QString Valor) {
    ui->textBrowser->setFontPointSize(size);
    ui->textBrowser->append(Valor);
}

void DialogManual::limpa(void) {
    ui->textBrowser->setText("");
}

void DialogManual::showFile(QString Arquivo) {
    QFile thisfile(Arquivo);           //sets file for filename

    if(!thisfile.open(QIODevice::ReadOnly))
        QMessageBox::information(0,"info",thisfile.errorString()); //creates error message if can't open file

    QTextStream in(&thisfile);                                     //grabs text from file
    ui->textBrowser->setText(in.readAll());            //sets text of textbrowser to file text
}
