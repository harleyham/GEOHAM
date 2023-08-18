#include <QString>
#include <QFile>

#include "dialog_rename.h"
#include "ui_dialog_rename.h"

Dialog_Rename::Dialog_Rename(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Rename)
{
    ui->setupUi(this);

    ui->lineEdit->setReadOnly(false);
    ui->lineEdit->setAlignment(Qt::AlignRight);

    ui->lineEdit->setWhatsThis("Novo nome do arquivo");
    ui->buttonBox->setWhatsThis("Confirma ou rejeita alteração");

}

Dialog_Rename::~Dialog_Rename()
{
    delete ui;
}

void Dialog_Rename::Inicializa(QString &Name) {

    Filename = Name.section('.',0,0);
    Ext = Name.section('.',1);

    ui->lineEdit->setText(Filename);
}

void Dialog_Rename::on_buttonBox_accepted()
{

}


void Dialog_Rename::on_buttonBox_rejected()
{

}


QString Dialog_Rename::getName()
{
    return (ui->lineEdit->text()+"."+Ext);
}
