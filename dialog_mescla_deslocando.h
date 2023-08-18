#ifndef DIALOG_MESCLA_DESLOCANDO_H
#define DIALOG_MESCLA_DESLOCANDO_H

#include <QDialog>

namespace Ui {
class Dialog_Mescla_Deslocando;
}

class Dialog_Mescla_Deslocando : public QDialog
{
    Q_OBJECT

public:

    bool            Processa;
    int             Num_Files,Imagem1,Imagem2;
    double          Angulo;


    void        print_NumFile(int Num);
    void        setNumFiles(int Num);
    int         get_Posicao();
    double      get_Angulo();
    void        set_Angulo(double Ang);

    explicit Dialog_Mescla_Deslocando(QWidget *parent = nullptr);
    ~Dialog_Mescla_Deslocando();

private slots:
    void on_bt_Proximo_clicked();

private:
    Ui::Dialog_Mescla_Deslocando *ui;

signals:
    bool    Next();

};

#endif // DIALOG_MESCLA_DESLOCANDO_H
