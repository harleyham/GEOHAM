#ifndef DIALOG_BLEND_H
#define DIALOG_BLEND_H

#include <QDialog>

namespace Ui {
class Dialog_Blend;
}

class Dialog_Blend : public QDialog
{
    Q_OBJECT

public:

    void Print(QString Valor);
    void Print_File1(QString Valor);
    void Print_File2(QString Valor);
    void clear(void);
    void Ajusta_Parametros(void);
    bool Modo_de_Operacao(void);

    void Print_Slider(void);

    int Get_Value();

    explicit Dialog_Blend(QWidget *parent = nullptr);
    ~Dialog_Blend();

private slots:


    void on_horizontalSlider_valueChanged(int value);

    void on_Botao_Sobrepor_clicked();

    void on_Botao_Blend_clicked();

    void on_Botao_Usar_clicked();

    void on_Botao_Cancel_clicked();

private:
    Ui::Dialog_Blend *ui;

signals:
    bool    Alterado();
    bool    Sobrepor();
    bool    Blend();
    bool    Cancel_Blend();
    bool    Usar_Blend();

};

#endif // DIALOG_BLEND_H
