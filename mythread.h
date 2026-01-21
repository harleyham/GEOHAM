#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QtCore>
#include "geo_exif_xmp.h"

class MyThread : public QThread
{
    Q_OBJECT

public:
    MyThread();
    void        run();
    QString     name;
    QString     NameKMZ;
    QString     NameTIF;
    int         Cor_Nodata;

};

class MyThread_Merge : public QThread
{
    Q_OBJECT

public:
    MyThread_Merge();
    void            run();
    QString         name;
    QString         NameTIF;
    QString         NameKMZ;
    int             Cor_Nodata;
    bool            Mesmo_Dir;
    Imagem_Total    Imagem_Composta;
    int             Contador_Escala;

signals:
    void AtualizaProgressBarMesclar();

};

class MyThread_ICE_TIF : public QThread
{
    Q_OBJECT

public:
    MyThread_ICE_TIF();
    void                run();
    QString             Output_VRT;
    QString             NameICE;
    QString             NameTIF;
    int                 Cor_Nodata;
    Geometria_Imagem    Geo_ICE,Geo_Tiff;
};


class MyThread_RES : public QThread
{
    Q_OBJECT

public:
    MyThread_RES();
    void                run();
    QString             NameInput;
    QString             NameOutput;
    int                 XSize;
};



#endif // MYTHREAD_H
