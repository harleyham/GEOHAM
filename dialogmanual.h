#ifndef DIALOGMANUAL_H
#define DIALOGMANUAL_H

#include <QDialog>

namespace Ui {
class DialogManual;
}

class DialogManual : public QDialog
{
    Q_OBJECT

public:
    explicit DialogManual(QWidget *parent = nullptr);
    ~DialogManual();

    void print(int size,QString Valor);
    void limpa(void);
    void showFile(QString Arquivo);

private:
    Ui::DialogManual *ui;
};

#endif // DIALOGMANUAL_H
