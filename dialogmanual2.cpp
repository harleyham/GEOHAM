#include "dialogmanual2.h"
#include "ui_dialogmanual2.h"

DialogManual2::DialogManual2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogManual2)
{
    ui->setupUi(this);
}

DialogManual2::~DialogManual2()
{
    delete ui;
}
