
#include <QSlider>

#include "dialog_blend.h"
#include "ui_dialog_blend.h"

Dialog_Blend::Dialog_Blend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Blend)
{
    ui->setupUi(this);

    ui->Valor_Slider->setAlignment(Qt::AlignRight);

    ui->Valor_Slider->setVisible(false);

//    connect(Dialog_Blend, SIGNAL(Alterado()), this, SLOT(Print_Valor(void)));

}

Dialog_Blend::~Dialog_Blend()
{
    delete ui;
}

void Dialog_Blend::Print(QString Valor){
    ui->Output_txt->append(Valor);
}

void Dialog_Blend::clear(void){
    ui->Output_txt->clear();
}

void Dialog_Blend::Print_File1(QString Valor){
    ui->Image_Fundo->setText(Valor);
}

void Dialog_Blend::Print_File2(QString Valor){
    ui->Image_Processada->setText(Valor);
}

void Dialog_Blend::Ajusta_Parametros() {
    ui->horizontalSlider->setTickPosition( QSlider::TicksBelow );
    ui->horizontalSlider->setTickInterval(1);
    ui->horizontalSlider->setRange(0,20);
    ui->horizontalSlider->setValue(10);
    ui->horizontalSlider->setSingleStep(1);
    ui->horizontalSlider->setPageStep(2);
    ui->horizontalSlider->setEnabled(false);
    ui->Valor_Slider->setEnabled(false);
    ui->Botao_Sobrepor->setChecked(true);
    ui->Botao_Blend->setChecked(false);
}

/*
void Dialog_Blend::on_horizontalSlider_sliderMoved(int position)
{
//    ui->Valor_Slider->setText(QString::number(position * 10) + " %");
    emit Alterado();
}
*/
void Dialog_Blend::Print_Slider(void) {
    int Val;

    Val = ui->horizontalSlider->value();
    ui->Valor_Slider->setText(QString::number(Val * 5) + " %");
}

void Dialog_Blend::on_horizontalSlider_valueChanged(int value)
{
    Print_Slider();
    emit Alterado();
}

int Dialog_Blend::Get_Value() {
    return (ui->horizontalSlider->value() * 5);
}

void Dialog_Blend::on_Botao_Sobrepor_clicked()
{
    ui->horizontalSlider->setEnabled(false);
    ui->Valor_Slider->setEnabled(false);
    emit Sobrepor();
}


void Dialog_Blend::on_Botao_Blend_clicked()
{
    ui->horizontalSlider->setEnabled(true);
    ui->Valor_Slider->setEnabled(true);
    emit Blend();
}


bool Dialog_Blend::Modo_de_Operacao(void) { // Retorna true se for modo Blend, false se for modo soprepor
    if (ui->Botao_Blend->isChecked()) return (true); else return (false);
}

void Dialog_Blend::on_Botao_Usar_clicked()
{
    close();
    emit Usar_Blend();
}

void Dialog_Blend::on_Botao_Cancel_clicked()
{
    close();
    emit Cancel_Blend();
}
