#include "dialogexif.h"
#include "ui_dialogexif.h"

DialogEXIF::DialogEXIF(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEXIF)
{
    ui->setupUi(this);

    ui->textBrowser->append("teste\n");

}

DialogEXIF::~DialogEXIF()
{
    delete ui;
}

void DialogEXIF::print(QString Valor) {
    ui->textBrowser->setText(Valor);
}
