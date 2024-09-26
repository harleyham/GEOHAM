#ifndef DIALOGEXIF_H
#define DIALOGEXIF_H

#include <QDialog>

namespace Ui {
class DialogEXIF;
}

class DialogEXIF : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEXIF(QWidget *parent = nullptr);
    ~DialogEXIF();

    void print(QString Valor);

private:
    Ui::DialogEXIF *ui;
};

#endif // DIALOGEXIF_H
