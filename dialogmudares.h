#ifndef DIALOGMUDARES_H
#define DIALOGMUDARES_H

#include <QDialog>

namespace Ui {
class DialogMudaRes;
}

class DialogMudaRes : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMudaRes(QWidget *parent = nullptr);
    ~DialogMudaRes();

    void print(void);

private slots:

    void on_XRes_M_editingFinished();

    void on_YRes_M_editingFinished();

    void on_CM_PX_M_editingFinished();

    void on_buttonBox_accepted();

private:
    Ui::DialogMudaRes *ui;
};

#endif // DIALOGMUDARES_H
