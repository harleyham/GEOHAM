#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfoList>
#include <QListWidgetItem>

#include <QAction>
#include <QMenu>

#include <QGuiApplication>
#include <QQmlApplicationEngine>


#include "dialogexif.h"
#include "dialogpontos.h"
#include "dialogsobre.h"
#include "dialogmanual.h"
#include "dialogmanual2.h"
#include "dialogmudares.h"
#include "dialog_blend.h"
#include "dialog_conv.h"
#include "dialog_desloca_img.h"
#include "dialog_mescla_deslocando.h"

#include "geo_exif_xmp.h"

#include "gdal_version.h"
#include "gdal_priv.h"

#include "mythread.h"

#include "stdoutredirector.h"

#include "pdf.h"

#define VER 0.66.7
#define HAM_VER "HAM_Geo_0.667"
#define HAM_NUM_IMAGES "HAM_Num_Images"
#define HAM_ANGULO_IDA "HAM_Angulo_IDA"
#define HAM_ANGULO_VOLTA "HAM_Angulo_VOLTA"
#define HAM_EXIF_MAKE "HAM_EXIF_Make"
#define HAM_EXIF_MODEL "HAM_EXIF_Model"
#define HAM_SENTIDO "HAM_Sentido"
#define HAM_PUL "HAM_PUL"
#define HAM_YAWDEGREE "HAM_YawDegree"
#define HAM_ALTITUDE "HAM_Altitude"

#define MERGE_TEMP1 "Merge_Temp1.tif"
#define MERGE_TEMP2 "Merge_Temp2.tif"
#define MERGE_TEMP3 "Merge_Temp3.tif"

#define ERROR_ANGLE 1e5

#define GDAL_WRITE 1

#define GROSSURA 8

#define PI 3.14159265

#define PI_180 0.0174532925

#define VAI 1
#define VOLTA 2
#define CURVA 4
#define ESCOLHIDOS 1000

#define CONFIG_THUMB 0
#define CONFIG_PREENCHE_POLIGONO 1
#define CONFIG_NODATA 2
#define CONFIG_REF 3
#define CONFIG_ALG 4
#define CONFIG_BORDAS 5
#define CONFIG_MOVER 6
#define CONFIG_MESCLAGEM 7
#define CONFIG_PUL 8
#define CONFIG_CURVA 9
#define CONFIG_MILHAR 10
#define CONFIG_TODAS 11
#define CONFIG_LINHAS 12
#define CONFIG_CIRCULOS 13
#define CONFIG_POLIGONO 14
#define CONFIG_REDUC 15
#define CONFIG_ESCALA 16
#define CONFIG_ALTVOO 17
#define CONFIG_ALTVOO_VALOR 18

#define CONFIG_DIR 19 // Este deve ser sempre o último


#define CONFIG_THUMB_TXT "THUMB"
#define CONFIG_PREENCHE_POLIGONO_TXT "PREENCHEPOLIGONO"
#define CONFIG_NODATA_TXT "NODATA"
#define CONFIG_REF_TXT "REF"
#define CONFIG_ALG_TXT "ALG"
#define CONFIG_BORDAS_TXT "BORDAS"
#define CONFIG_MOVER_TXT "MOVER"
#define CONFIG_MESCLAGEM_TXT "MESCLAGEM"
#define CONFIG_PUL_TXT "PUL"
#define CONFIG_CURVA_TXT "CURVA"
#define CONFIG_MILHAR_TXT "MILHAR"
#define CONFIG_TODAS_TXT "TODAS"
#define CONFIG_LINHAS_TXT "LINHAS"
#define CONFIG_CIRCULOS_TXT "CIRCULOS"
#define CONFIG_POLIGONO_TXT "POLIGONO"
#define CONFIG_REDUC_TXT "REDUC"
#define CONFIG_ESCALA_TXT "ESCALA"
#define CONFIG_ALTVOO_TXT "ALTVOO"
#define CONFIG_ALTVOO_VALOR_TXT "ALTVOOVALOR"

#define CONFIG_DIR_TXT "DIR" // Este deve ser sempre o último


class QPlainTextEdit;

class Info_Merge{
public:
    QString FileName;
    double  Lat,Lon;
};

class Plot_Coords_Tipo{
public:
    QList<QString>      FileName;
    QList<double>       Lat,Lon,Alt,Gimbal,Angle,Lat_A,Lon_A;
    QList<double>       mx,my;
    QList<int>          Iw,Ih;
    QList<int>          Direcao;
    double              Media_Ida,Media_Volta;
    int                 Num_Ida,Num_Volta,Ida,Volta,Curvas;
    bool                Par;
    int                 Ultimo_Primeira,Primeiro_Ultima;
    QString             Make,Model;


    Plot_Coords_Tipo();
    void clear(void);
};

class Plot_Coords{
public:
    Plot_Coords_Tipo    PROC;
    Plot_Coords_Tipo    IMGS;
    int                 Tipo;
    bool                ShowAngle;

    Plot_Coords();
};



class Info_Dir{
public:
    QString                         Diretorio_EXE,Diretorio,DiretorioProc,DiretorioThumb,Diretorio_Img_Original,PyGeoRef,TXT_File,Diretorio_Movidas;
    QString                         DiretorioResult,Diretorio_Foco,Imagem_Foco;
    QList<QString>                  ListadeArquivos_MergeResult;
    QList<QString>                  ListadeArquivos_MergeProc;
    std::string                     Name_Imagem1;
    QFileInfoList                   ListadeArquivos,ListadeArquivosProc,ListadeArquivosJPG,ListadeArquivosResult,ListadeArquivosJPG_selecionados;
    QFileInfoList                   ListadeArquivos_Coord;
    QList<Info_Merge>               Merge;
    int                             Reducao;
    double                          Escala1,Escala2;

    QList< std::complex<double> >   Coords;
//    Plot_Coords                     Coords_IMG,Coords_PROC;

    Info_Dir();
} ;


class Arquivo_Config_File {
public:
//    bool                  Thumb,Poligono,Cor_NODATA,Referencia_Angulo,Algoritmo_Mesclagem,Gerar_Bordas,Mover_Fora_Padrao,Mesclagem_Todas,Primeira_Ultima_Linha,Drone_em_curva,Separacao_Milhar;
    bool                  TemDir;
    std::string           Dir;
    QList<QString>        Tipos;
    QList<int>            Valores;

    Arquivo_Config_File();
};


class Geometry_Widgets {
public:
    QByteArray              savedGeometry;
    QByteArray              savedGeometry_txt_Output;
    QByteArray              savedGeometry_tablePontos;
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QProcess                *myProcess;


    EXIF_XMP_Info           Info_Img;

    Geometria_Imagem        Geometria_Img;
    Geometria_Imagem        Geometria_ImgProc;

    QList<Geometria_Imagem> Geo_JPG;
    Geometria_Imagem        Geo_Mesclado;

    cv::Mat                 Destino;
    QString                 Geo_Tiff_Name;
    Imagem_Total            Imagem_Composta;

    Info_Dir                Informacao,Informacao2;

    QString                 dirOpen;
    QString                 dirOpenProc;
    QString                 dirOpenResult;
    QString                 dirOpenThumb;
    QTimer                  *timerOpen;
    bool                    CancelarProc;
    bool                    Nao_ProgressBar_Result = false;


    int                     READCOUNT;
    int                     FPS_open,numFrames, frameW,frameH;
    float                   Temp2;
    MyThread                mThread1;
    MyThread_Merge          mThread_Merge;
    MyThread_ICE_TIF        mThread_ICE_TIF;
    MyThread_RES            mThread_RES;
    MyThread_Ajuda          mThread_Ajuda;

    QElapsedTimer           timer2;
//    QTime                   timer2;


    QAction                 *actSelectAllImgs , *actDeSelectAllImgs , *actProcess_1, *actInfo, *actShowCoord = NULL, *act_KML;
    QMenu                   *contextMenu = NULL;

    QAction                 *actSelectAllProc , *actDeSelectAllProc , *actDelete , *actMesclar , *actInfoProc, *actKMZProc, *actShowCoordProc = NULL, *act_KML_Proc;
    QMenu                   *contextMenuProc = NULL;

    QAction                 *actSelectAllResult , *actDeSelectAllResult, *actDelete_Result, *actKMZResult, *actRESResult, *actMesclarResult, *actRenameResult, *actInfoResult;
    QMenu                   *contextMenuResult = NULL;

    QPixmap                 thumb_Earth,thumb_Mundo;

    QPixmap                 iconCheckpix,iconCheckpixOff;
    QIcon                   iconCheck;

    Geometria_Imagem        Blend_Calculada,Sobrepor_Calculada;

    bool                    Calculou_Blend = false, Calculou_Sobrepor = false;

    bool                    CapturaStdout = false;
    QString                 TextStdout;
    QTimer                  *TimerStdout;
    float                   ProgressBarValue;


    QQmlApplicationEngine   engine;

    Arquivo_Config_File     Config;

    PDF_WINDOW              *dialog_PDF = NULL;

    Geometry_Widgets        Geometria_Widgets;

    bool                    Changed_Screen = false;

    QString                 str_Ativado = tr("ATIVADO");
    QString                 str_Desativado = tr("DESATIVADO");


private slots:

    static void on_mouse_OpenFile(int event, int x, int y, int, void *);
    static void on_mouse_ImagemRotacionada(int event, int x, int y, int, void *);
    static void on_mouse_Original_JPG(int event, int x, int y, int flags, void *);
    static void on_mouse_Original_TIF(int event, int x, int y, int flags, void *);
    static void on_mouse_Referencia_TIF(int event, int x, int y, int flags, void *);
    static void on_mouse_Desloca_TIF_1(int event, int x, int y, int flags, void *);
    static void on_mouse_Desloca_TIF_2(int event, int x, int y, int flags, void *);
    static void on_mouse_Desloca_TIF_N(int event, int x, int y, int flags, void *);
    static void on_mouse_ArquivosJPG(int event, int x, int y, int flags, void *);



    static void on_mouse_Teste(int event, int x, int y, int, void *);

    void on_btOpenFile_clicked();

    void on_btOpenDirectory_clicked();

    void on_txt_Reducao_returnPressed();

    void on_bt_Reduzir_clicked();

    void on_bt_ProcessarImgs_clicked();                                                                      // PROCESSAMENTO.cpp

    void processarImgs();                                                                                    // PROCESSAMENTO.cpp

    void on_bt_Dir_Proc_clicked();

    void Lista_Files_Proc();

    void Lista_Files_Result();

    void Lista_Files_Imgs(bool Novo = false);                                                                                // LISTA_FILES.cpp

    bool Cria_Dir_Proc();                                                                                   // DIR_PRINCIPAL.cpp

    void Cria_Dir_Result();

    void Cria_Dir_Thumb();

    void Cria_Dir_Movidas();                                                                                // DIR_PRINCIPAL.cpp

    void on_btCmd_clicked();

    void on_bt_Cancelar_Proc_clicked();

    void on_bt_Lista_Arquivos_clicked();

    void on_bt_EXIF_XMP_List_clicked();

    void on_actionTIFF_KMZ_triggered();                                                                     // TIFF_KMZ.cpp

    void on_actionHarley_2_changed();

    void on_actionMiranda_2_changed();

    void on_actionMiranda_2_triggered();

    void on_actionHarley_2_triggered();

    void on_actionMuda_resolucao_triggered();

    void Rotaciona(cv::Mat Img, cv::Mat &dst, double angle);                                        // geo_exif_xmp.CPP
    void Rotaciona(cv::Mat Img, cv::Mat &dst, double angle, int cx, int cy);

    void Rotaciona_Imagem(Geometria_Imagem Geo,cv::Mat imageRead, cv::Mat &Destino);                                     // geo_exif_xmp.CPP

    void on_bt_Mesclar_clicked();

    void on_bt_Teste_clicked();
    void on_bt_Teste_Cancel();

    bool DecodeXMP(QStringList &stringList, GDALDataset *poSrcDS);                                  // geo_exif_xmp.CPP

    bool RetiraParenteses(char *Dest,char *Str);

    double getLONfromEXIF(GDALDataset *poSrcDS);

    double getLATfromEXIF(GDALDataset *poSrcDS);

    double getANGLEfromEXIF(GDALDataset *poSrcDS);                                                  // geo_exif_xmp.CPP

    double GetXMPItem(QStringList Lista,QString Item);

    void on_bt_DialogPontos_clicked();

    void on_list_Files_itemDoubleClicked(QListWidgetItem *item);

    void on_txt_Escala_returnPressed();

    void on_txt_Escala_textEdited(const QString &arg1);

    void on_txt_Escala_editingFinished();

    void on_txt_Reducao_editingFinished();

    void AbreArquivo(QFileInfo CompleteName, bool Clear_txt_Output, bool UsaEscala, bool Mesmo_Dir = false);

    void on_list_Files_Proc_itemDoubleClicked(QListWidgetItem *item);

    bool on_bt_limpaProc_clicked();

    void on_actionSobre_triggered();

    void on_actionManual_triggered();

    void on_actionICE_JPG_TIF_triggered();

    void on_actionTIFs_ICE_JPG_triggered();

    void on_actionEspecificar_2_triggered();

    void on_actionValor_triggered();

    void on_actionThread_2_triggered();

    void Gera_Merge(int Cor_Nodata);

    void on_checkBox_KMZ_clicked();

    void on_checkBox_GTiff_clicked();

    void on_list_Files_Result_itemDoubleClicked(QListWidgetItem *item);

    bool Eh_GeoTiff(QString InputFile, int &No_Data);                                                               // GDAL_HAM.cpp

    void Size_GeoTiff(QString InputFile, bool Print, QString &Output);                                              // GDAL_HAM.cpp

    void Size_ofJPG(QString InputFile, bool Print, QString &Output);                                                // GDAL_HAM.cpp

    void Muda_resolucao(int XSize, QString InputTif, QString OutputTif);

    int NovoXRes(void);

    void on_actionAbrir_triggered();

    void on_list_Files_itemClicked(QListWidgetItem *item);

    void on_actionPontos_triggered();

    void on_list_Files_Proc_itemClicked(QListWidgetItem *item);

    void on_list_Files_Result_itemClicked(QListWidgetItem *item);

    void on_actionEXIF_XMP_triggered();

    void Le_Geo_from_file(QFileInfo CompleteName);

    void on_actionCapturar_Pontos_triggered();

    void on_actionGerar_GeoTiff_triggered();

    void on_actionTIFF_JPG_triggered();

    void on_action2_pontos_triggered();

    void on_actionCaminho_triggered();

    void on_action_rea_triggered(bool No_Print = false);

    void on_actionLimpar_triggered(bool ShowImage = true);

    void on_actionThumbnail_triggered();

    void on_Limpa_Quadro_clicked();

    void on_actionPortugu_s_triggered();

    void on_actionIngl_s_triggered();

    void Monta_Tabela_Inicial();

    void Dist_2Pontos(void);

    void on_checkBox_Linhas_clicked();

    void on_pushButton_clicked();

    void on_checkBox_Circle_clicked();

    void Draw_Cel2();

    void on_CheckBox_Poligono_clicked();

    float Calc_Dist();

    float Calc_Perimetro(void);

    float Calc_Area(void);

    void on_actionPreenche_Pol_gono_triggered();

    void writeGeoInfo(QString output,cv::Mat output_image,char **papszMetadata,Geometria_Imagem &Geometria_ImgProc);
    void writeGeoInfo(QString output,cv::Mat output_image,char **papszMetadata,double *Transform);
    void writeGeoInfo(QString output,cv::Mat output_image,char **papszMetadata,char **papszMetadatadomain,Geometria_Imagem &Geometria_ImgProc);

    void on_actionBranco_triggered();

    void on_actionPreto_triggered();

    void MAT2GDAL(QString output, cv::Mat output_image, char **papszMetadata, Geometria_Imagem &Geometria_ImgProc, Imagem_Total *Imagem = NULL);
    void MAT2GDAL(QString output,cv::Mat output_image,char **papszMetadata,double *Transform);

    void GDAL2MAT(GDALDataset *,cv::Mat &);

    void on_actionSair_triggered();

    void closeEvent(QCloseEvent *event);

    void on_actionDrone_triggered();

    void on_actionGimbal_triggered();

    void on_actionGDAL_triggered();

    void on_actionInterno_triggered();

    void Gera_Merge_Interno(int Cor_NoDATA);

    void on_actionBordas_na_imagens_processadas_triggered();

    void eraseItemProc();

    void SelectAll_Proc();

    void DeSelectAll_Proc();

    void TifKMZProc();                                                                      // TIFF_KMZ.cpp

    void eraseItem_Result();

    void SelectAll_Result();

    void DeSelectAll_Result();

    void TifKMZResult();                                                                    // TIFF_KMZ.cpp

    void Cria_Contex_Menu_Imgs();

    void Cria_Contex_Menu_Imgs_2();

    void Cria_Contex_Menu_Proc();

    void Cria_Contex_Menu_Result();

    void get_Info_Img();                                                                     // lista_Files.cpp

    void get_Info_Proc();                                                                    // lista_Files.cpp

    void get_Info_Result();                                                                  // lista_Files.cpp

    void processa_Imgs();

    void SelectAll_Imgs();

    void DeSelectAll_Imgs();

    void MesclarItens(QString Complete);
    void MesclarItens(QString Complete,int Sentido);

    void executaMesclar(QString Complete);

    void Mesclar_Result();

    void Mesclar_Prov();                                                                    // mesclagem.CPP

    void Le_imagem_JPG();                                                                   // processamento.CPP

    void processarImgsProv(double Angulo);                                                  // processamento.CPP
    void processarImgsProv0(double Angulo);                                                  // processamento.CPP

    void Seleciona_Imagens_para_Mesclar(int Sentido);                                       // MESCLAGEM.cpp

    void Print_Caracteristicas_Mesclagem(QString File,int Sentido);

    void Rename_Result();

    void getEXIF_XML_Info(QString File);

    void on_actionCortarTIFF_triggered();

    void Grava_Thumb(QString Tiff_Writed);

    void TifKMZ(bool Mesmo_Dir, bool Proc = false);                                                                // TIFF_KMZ.cpp

    void KMZTiff(bool Mesmo_Dir);

    int keyPressEvent_H(QKeyEvent *event);

    void on_actionKMZ_TIFF_triggered();

    void mudaRES_Result();

    void Grava_KMZ_from_file();

    bool Pergunta_Delete();

    void Gera_Retangulo();

    void on_bt_Mostar_Imagem_clicked();

    void on_actionDesativado_triggered();

    void on_actionOriginais_triggered();

    void on_actionProcessadas_triggered();

    void ShowCoord();

    void ShowCoordProc();

    void on_actionRaio_30_pts_triggered();

    void Le_Geo_from_file2(Geometria_Imagem &Ref);                                              // MAINWINDOW.cpp

    void Calc_Blend_Images(void);

    void Calc_Sobrepor_Images(void);

    void Show_Sobrepor_Images(void);

    void Blend_Ok(void);

    cv::Mat equalizeIntensity(const cv::Mat& inputImage);

    void on_actionCarregar_triggered();                                                         // MAINWINDOW.cpp

    void on_actionMostrar_triggered();

    void Get_Info_Window(QString NameWindow,Geometria_Imagem &Geometria);

    bool Contido_Referencia(Geometria_Imagem &Geometria_Ref,Geometria_Imagem Geometria);

    void on_actionUsar_triggered();

    void on_actionSalvar_TIF_mixado_triggered();

    void Grava_KML(bool Track, bool Extrude, bool Shadow, bool Pontos, bool Tempo, bool Poligono, double Altitude, QList <std::complex <double> > Pontos_Coords, QString Dir = "./");

    void Le_KML_FromFile(QString Filename);

    void Le_KML(QWidget *parent);

    void on_actionSave_KML_triggered();

    void on_actionLoad_KML_triggered();

    void Grava_KML_Voo();

    void Grava_KML_Voo_Proc();

    void Calc_Imagem_Blank(double Res, int &cols, int &rows);

    bool GetLatLonfromINFO(QString Input, double &Lat, double &Lon);

    void on_actionConversor_Coordenadas_triggered();                                                    // CONVERSOR_COORDENADAS.cpp

    void Inicializa(UTM_GEO &Val,GeographicLib::GeoCoords &Val_C);

    void on_actionMover_imagens_triggered();

    void ResetProgressBarMesclar_EXEC();                                                                // PROCESSAMENTO.cpp
    void ActiveProgressBarMesclar_EXEC();                                                               // PROCESSAMENTO.cpp
    void AtualizaProgressBarMesclar_Merge();

    void readData();
    void onTimeout();
    void Redirector();

    void Desloca_Img();
    void Cancel_Desloca(bool Cancelar = true);
    void Ok_Desloca();
    void Desloca_Digita();
    void Desloca_Img_Ref();
    void Desloca_2Pontos();

    void on_actionDesloca_Imagem_triggered();

    void on_actionCoordenadas_Centrais_na_tabela_triggered();

    void Le_Imagem_TIF(QString InputTif,Geometria_Imagem &Geo_Tiff);                                    //

    void on_actionTodas_as_imagens_triggered();

    void on_actionSendido_nico_1_triggered();

    void on_actionSentido_unico_2_triggered();

    void on_actionUsar_imagens_do_drone_em_curva_triggered();

    void on_actionUsar_imagens_da_primeira_e_ltima_linha_triggered();

    void on_actionMostrar_informa_o_dire_es_triggered();

    void on_pushButton_2_clicked();

    void on_actionM_dia_triggered();

    void on_actionMostrar_informa_o_pilha_de_imagens_triggered();

    void MergeSequencia();

    void Grava_Config_File();

    void Le_Config_File();

    void Exec_Config_File();

    void on_bt_MudaResolucao_clicked();


    void on_action1280x720_triggered();

    void on_action1920x1080_triggered();

    bool Load_Widgets_Settings(bool Res);

    void GetWidgetGeometry(QXmlStreamReader &xmlReader, double &x, double &y, double &width, double &height);

    void sscreenChanged(QScreen* Screen);

    void on_actionPortugu_s_2_triggered();

    void on_actionIngl_s_2_triggered();

public slots:
    void imprimeFimKMZ(void);                                                                           // PROCESSAMENTO.cpp
    void imprimeFimMerge(void);                                                                         // PROCESSAMENTO.cpp
    void imprimeFimICE_TIFF(void);
    void imprimeFimRES(void);
    void updateTxt(void);

    void Valor_Slider_Changed();
    void Valor_Slider_Changed(cv::Mat &Image);

private:
    StdoutRedirector    *redirector = NULL;
    QPlainTextEdit      *edit;

    DialogEXIF      *dialogEXIF;
    DialogPontos    *dialogPontos;
    DialogSobre     *dialogSobre;
    DialogManual    *dialogManual;
    DialogMudaRes   *dialogMudaRes;

    DialogManual2   *dialogManual2;

    Dialog_Conv     *dialogConv;

    QPdfWidget      *m_pPdfWidget;

    Ui::MainWindow  *ui;

    Dialog_Blend    *dialogBlend;
    Dialog_Mescla_Deslocando *dialogMesclaDesloca;


signals:
    void FimKMZ();
    void ResetProgressBarMesclar();

};


class message_function
{
public:

    int Raio_Circulo;

    void Print(Ui::MainWindow *ui,std::string Valor);
    void PrintStr(Ui::MainWindow *ui,QString Valor);
    void Table1(Ui::MainWindow* ui, int Row, bool Insert);
    void Draw_Cel(void);
    void Gera_Retangulo(Ui::MainWindow* ui);
    void PreencheTable(Ui::MainWindow* ui);
    void Atualiza_Lista_Pontos_Apos_Move(Ui::MainWindow *ui);

    void Atualiza_Raio_Marcacao_Coordenadas(int Raio);
    int Valor_Raio_Marcacao_Coordenadas(void);
};


#endif // MAINWINDOW_H
