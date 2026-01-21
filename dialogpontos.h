#ifndef DIALOGPONTOS_H
#define DIALOGPONTOS_H

#include <QDialog>

namespace Ui {
class DialogPontos;
}

class DialogPontos : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPontos(QWidget *parent = nullptr);
    ~DialogPontos();

    void Print(QString Valor);
    void clear(void);

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    Ui::DialogPontos *ui;
};

#endif // DIALOGPONTOS_H
