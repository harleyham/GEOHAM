#include "dialogsobre.h"
#include "ui_dialogsobre.h"

DialogSobre::DialogSobre(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSobre)
{
    ui->setupUi(this);
}

DialogSobre::~DialogSobre()
{
    delete ui;
}

void DialogSobre::print(QString Valor) {
    ui->textBrowser->append(Valor);
}

void DialogSobre::limpa(void) {
    ui->textBrowser->setText("");
}
