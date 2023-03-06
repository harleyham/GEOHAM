#define _CRT_SECURE_NO_WARNINGS

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core.hpp>
#include <string>
#include <iostream>
#include <iomanip>
#include <cassert>

#include <QString>
#include <QStringList>
#include <QtGui>

#include <string>

#include "geo_exif_xmp.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#if defined(EXV_UNICODE_PATH) && defined(__MINGW__)
#undef  EXV_UNICODE_PATH
#endif

#ifdef  EXV_UNICODE_PATH
#define _tchar      wchar_t
#define _tstrcmp    wcscmp
#define _t(s)       L##s
#define _tcout      wcout
#define _tmain      wmain
#else
#define _tchar      char
#define _tstrcmp    strcmp
#define _t(s)       s
#define _tcout      cout
#define _tmain      main
#endif

//#define M_PI 3.1415926

void EXIF_XMP_Info::clear() {
  // Strings
  ImageDescription = "";
  Make = "";
  Model = "";
  Software = "";
  DateTime = "";
  DateTimeOriginal = "";
  DateTimeDigitized = "";
  SubSecTimeOriginal = "";
  Copyright = "";

  // Shorts / unsigned / double
  ByteAlign = 0;
  Orientation = 0;

  BitsPerSample = 0;
  ExposureTime = 0;
  FNumber = 0;
  ExposureProgram = 0;
  ISOSpeedRatings = 0;
  ShutterSpeedValue = 0;
  ExposureBiasValue = 0;
  SubjectDistance = 0;
  FocalLength = 0;
  FocalLengthIn35mm = 0;
  Flash = 0;
  FlashReturnedLight = 0;
  FlashMode = 0;
  MeteringMode = 0;
  ImageWidth = 0;
  ImageHeight = 0;

  // Geolocation
  GeoLocation.Latitude = 0;
  GeoLocation.Longitude = 0;
  GeoLocation.Altitude = 0;
  GeoLocation.AltitudeRef = 0;
  GeoLocation.DOP = 0;
  GeoLocation.LatComponents.degrees = 0;
  GeoLocation.LatComponents.minutes = 0;
  GeoLocation.LatComponents.seconds = 0;
  GeoLocation.LatComponents.direction = '?';
  GeoLocation.LonComponents.degrees = 0;
  GeoLocation.LonComponents.minutes = 0;
  GeoLocation.LonComponents.seconds = 0;
  GeoLocation.LonComponents.direction = '?';
  GeoLocation.Altitude_Rel = 0;
  GeoLocation.Direction_Gimbal = 0;

  // LensInfo
  LensInfo.FocalLengthMax = 0;
  LensInfo.FocalLengthMin = 0;
  LensInfo.FStopMax = 0;
  LensInfo.FStopMin = 0;
  LensInfo.FocalPlaneYResolution = 0;
  LensInfo.FocalPlaneXResolution = 0;
  LensInfo.FocalPlaneResolutionUnit = 0;
  LensInfo.Make = "";
  LensInfo.Model = "";
}


void Rotaciona2(cv::Mat src, cv::Mat &dst, double angle)
{

//    imshow("Original", src);
    int theta = 0;
    cv::Mat frame, frameRotated;
//    cout<<endl<<endl<<"Press '+' to rotate anti-clockwise and '-' for clockwise 's' to save" <<endl<<endl;

    int diagonal = (int)sqrt(src.cols*src.cols+src.rows*src.rows);
    int newWidth = diagonal;
    int newHeight =diagonal;

    int offsetX = (newWidth - src.cols) / 2;
    int offsetY = (newHeight - src.rows) / 2;
    cv::Mat targetMat(newWidth, newHeight, src.type());
    cv::Point2f src_center(targetMat.cols/2.0F, targetMat.rows/2.0F);


    while(1){
    src.copyTo(frame);
//    double radians = theta * M_PI / 180.0;
//    double sin = abs(std::sin(radians));
//    double cos = abs(std::cos(radians));

    frame.copyTo(targetMat.rowRange(offsetY, offsetY + frame.rows).colRange(offsetX, offsetX + frame.cols));
    cv::Mat rot_mat = getRotationMatrix2D(src_center, theta, 1.0);
    warpAffine(targetMat, frameRotated, rot_mat, targetMat.size());
     //Calculate bounding rect and for exact image
     //Reference:- https://stackoverflow.com/questions/19830477/find-the-bounding-rectangle-of-rotated-rectangle/19830964?noredirect=1#19830964
        cv::Rect bound_Rect(frame.cols,frame.rows,0,0);

        int x1 = offsetX;
        int x2 = offsetX+frame.cols;
        int x3 = offsetX;
        int x4 = offsetX+frame.cols;

        int y1 = offsetY;
        int y2 = offsetY;
        int y3 = offsetY+frame.rows;
        int y4 = offsetY+frame.rows;

        cv::Mat co_Ordinate = (cv::Mat_<double>(3,4) << x1, x2, x3, x4,
                                                y1, y2, y3, y4,
                                                1,  1,  1,  1 );
        cv::Mat RotCo_Ordinate = rot_mat * co_Ordinate;

        for(int i=0;i<4;i++){
           if(RotCo_Ordinate.at<double>(0,i)<bound_Rect.x)
             bound_Rect.x=(int)RotCo_Ordinate.at<double>(0,i); //access smallest
           if(RotCo_Ordinate.at<double>(1,i)<bound_Rect.y)
            bound_Rect.y=RotCo_Ordinate.at<double>(1,i); //access smallest y
         }

         for(int i=0;i<4;i++){
           if(RotCo_Ordinate.at<double>(0,i)>bound_Rect.width)
             bound_Rect.width=(int)RotCo_Ordinate.at<double>(0,i); //access largest x
           if(RotCo_Ordinate.at<double>(1,i)>bound_Rect.height)
            bound_Rect.height=RotCo_Ordinate.at<double>(1,i); //access largest y
         }

        bound_Rect.width=bound_Rect.width-bound_Rect.x;
        bound_Rect.height=bound_Rect.height-bound_Rect.y;

        cv::Mat cropedResult;
        cv::Mat ROI = frameRotated(bound_Rect);
        ROI.copyTo(cropedResult);

        imshow("Result", cropedResult);
        imshow("frame", frame);
        imshow("rotated frame", frameRotated);
        char k=cv::waitKey();
        if(k=='+') theta+=10;
        if(k=='-') theta-=10;
//        if(k=='s') imwrite("rotated.jpg",cropedResult);
        if(k==27) {
            dst = cropedResult;
            cv::destroyWindow("Result");
            cv::destroyWindow("frame");
            cv::destroyWindow("rotated frame");
            cv::destroyWindow("Original");
            break;
         }
    }
}


void MainWindow::Rotaciona_Imagem(Geometria_Imagem Geo,cv::Mat imageRead, cv::Mat &Destino)
{
    Geometria_ImgProc = Geo;
    double angle = -Geometria_ImgProc.Gimbal_Angle; //-Info_Img.GeoLocation.Direction_Gimbal;

    Rotaciona(imageRead,Destino,angle);

    double xl,yl, GSDm, Iw2, Ih2;
//    double sin_alfa,cos_alfa;

    Geometria_ImgProc.Iw = Destino.cols;
    Geometria_ImgProc.Ih = Destino.rows;
    Iw2 = Destino.cols / 2;
    Ih2 = Destino.rows / 2;

    GSDm = Geometria_ImgProc.GSD / 100.0;

    xl = Geometria_ImgProc.CcUTM.Easting() - GSDm * Iw2;
    yl = Geometria_ImgProc.CcUTM.Northing() - GSDm * Ih2;
    Geometria_ImgProc.CUTM.Reset(Geometria_ImgProc.CcUTM.Zone(),Geometria_ImgProc.CcUTM.Northp(),xl,yl);

    xl = Geometria_ImgProc.CcUTM.Easting() + GSDm * Iw2;
    yl = Geometria_ImgProc.CcUTM.Northing() + GSDm * Ih2;
    Geometria_ImgProc.BUTM.Reset(Geometria_ImgProc.BUTM.Zone(),Geometria_ImgProc.CcUTM.Northp(),xl,yl);

    xl = Geometria_ImgProc.CcUTM.Easting() - GSDm * Iw2;
    yl = Geometria_ImgProc.CcUTM.Northing() + GSDm * Ih2;
    Geometria_ImgProc.AUTM.Reset(Geometria_ImgProc.CcUTM.Zone(),Geometria_ImgProc.CcUTM.Northp(),xl,yl);

    xl = Geometria_ImgProc.CcUTM.Easting() + GSDm * Iw2;
    yl = Geometria_ImgProc.CcUTM.Northing() - GSDm * Ih2;
    Geometria_ImgProc.DUTM.Reset(Geometria_ImgProc.BUTM.Zone(),Geometria_ImgProc.CcUTM.Northp(),xl,yl);
}


void MainWindow::Rotaciona(cv::Mat Img, cv::Mat &dst, double angle)
{
// get rotation matrix for rotating the image around its center in pixel coordinates
    cv::Point2f center((Img.cols-1)/2.0, (Img.rows-1)/2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
// determine bounding rectangle, center not relevant
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), Img.size(), angle).boundingRect2f();
// adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - Img.cols/2.0;
    rot.at<double>(1,2) += bbox.height/2.0 - Img.rows/2.0;
    cv::Scalar Cor;
    if (ui->actionBranco->isChecked()) {
        Cor = cv::Scalar(255,255,255);
    } else {
        Cor = cv::Scalar(0,0,0);
    }
    cv::warpAffine(Img, dst, rot, bbox.size(),cv::INTER_LINEAR,cv::BORDER_CONSTANT,Cor);
}

void MainWindow::Rotaciona(cv::Mat Img, cv::Mat &dst, double angle, int cx, int cy) {
    // get rotation matrix for rotating the image around its center in pixel coordinates
        cv::Point2f center(cx, cy);
        cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle, center not relevant
        cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), Img.size(), angle).boundingRect2f();
    // adjust transformation matrix
        rot.at<double>(0,2) += bbox.width/2.0 - Img.cols/2.0;
        rot.at<double>(1,2) += bbox.height/2.0 - Img.rows/2.0;
        cv::Scalar Cor;
        if (ui->actionBranco->isChecked()) {
            Cor = cv::Scalar(255,255,255);
        } else {
            Cor = cv::Scalar(0,0,0);
        }
        cv::warpAffine(Img, dst, rot, bbox.size(),cv::INTER_LINEAR,cv::BORDER_CONSTANT,Cor);
}


void Geometria_Imagem::Calculos(void) {
    double N,E;

    Sw = DistFocal * 34.6 / DistFocal35mm;
    GSD = (Sw * Alt * 100) / (DistFocal * Iw);
    Dw = (GSD * Iw) / 100.0;
    Dh = (GSD * Ih) / 100.0;

    CcUTM.Reset(Cc.real(),Cc.imag());

    dx = (Dw / 2.0);
    dy = (Dh / 2.0);

    N = CcUTM.Northing() + dy;
    E = CcUTM.Easting() - dx;
    AUTM.Reset(CcUTM.Zone(),CcUTM.Northp(),E,N);

    N = CcUTM.Northing() + dy;
    E = CcUTM.Easting() + dx;
    BUTM.Reset(CcUTM.Zone(),CcUTM.Northp(),E,N);

    N = CcUTM.Northing() - dy;
    E = CcUTM.Easting() - dx;
    CUTM.Reset(CcUTM.Zone(),CcUTM.Northp(),E,N);

    N = CcUTM.Northing() - dy;
    E = CcUTM.Easting() + dx;
    DUTM.Reset(CcUTM.Zone(),CcUTM.Northp(),E,N);
    mppx = Dw / Iw;
    mppy = - (Dh / Ih);

    if (Escala != 1.0) {
        Iw = Iw * Escala;
        Ih = Ih * Escala;
        GSD = 100.0 * Dw / Iw;
        mppx = Dw / Iw;
        mppy = - (Dh / Ih);
    }
}

void Geometria_Imagem::CopyTo(Geometria_Imagem &Dest,cv::Mat Image) {
    Dest.Iw = Iw;
    Dest.Ih = Ih;
    Dest.Cc = Cc;
    Dest.A = A;
    Dest.B = B;
    Dest.C = C;
    Dest.D = D;
    Dest.Dw = Dw;
    Dest.Dh = Dh;
    Dest.mppx = mppx;
    Dest.mppy = mppy;
    Dest.cmppx = cmppx;
    Dest.cmppy = cmppy;
    Dest.CcUTM = CcUTM;
    Dest.AUTM = AUTM;
    Dest.BUTM = BUTM;
    Dest.CUTM = CUTM;
    Dest.DUTM = DUTM;
    Dest.Cor_Nodata = Cor_Nodata;
    Image.copyTo(Dest.image);
}

///////////////////////////////////////////////////////////////////////////
bool MainWindow::RetiraParenteses(char *Dest,char *Str) {

    unsigned int i,j;
    j = 0;
    for (i = 0 ; i < strlen(Str) ; i++) {
        if ((Str[i] != '(') && (Str[i] != ')')) {
            Dest[j] = Str[i];
            j++;
        }
    }
    Dest[j] = 0;

    return true;
}

bool MainWindow::DecodeXMP(QStringList &stringList, GDALDataset *poSrcDS) {
    QString S2;
    char** papszIter;
    char** papszMetadataDomain = CSLDuplicate(poSrcDS->GetMetadataDomainList());

    stringList.clear();

    papszIter = papszMetadataDomain;
    while(papszIter && *papszIter)
    {
        S2 = *papszIter;

        if (S2.length()) {
            stringList.append(S2);
//            ui->txt_Output->append(S2);
        }
        papszIter++;
    }


    bool Ok = false;
    for (int i = 0 ; i < stringList.length() ; i ++) {
        if (stringList.at(i) == "xml:XMP") Ok = true;
    }

    if (!Ok) return false;

    char** papszMetadataXMP = CSLDuplicate(poSrcDS->GetMetadata("xml:XMP"));
    QString itemText = QString::fromUtf8(*papszMetadataXMP);

    stringList.clear();
    QTextStream textStream(&itemText);
    while (true)
    {
        QString line = textStream.readLine();
        if (line.isNull())
            break;
        else
//            if (line.contains("dji:")) stringList.append(line);
            stringList.append(line);
    }
    return true;
}


double MainWindow::GetXMPItem(QStringList Lista,QString Item) {

    int i,j,k;
    char strVal[20],Temp[100];
    strcpy(strVal,"0");
    for (i = 0 ; i < Lista.length() ; i++) {
        if (Lista.at(i).contains(Item)) {
            strcpy(Temp,Lista.at(i).toStdString().c_str());
            for (j = 0 ; j < strlen(Temp) ; j++) {
                if (Temp[j] == ':') break;
            }
            k = 0;
            for ( ; j < strlen(Temp) ; j++) {
                if ((isdigit(Temp[j])) || (Temp[j] == '.') || (Temp[j] == '-')) {
                    strVal[k] = Temp[j];
                    k++;
                }
            }
            strVal[k] = 0;
        }
    }
    return(atof(strVal));
}

bool MainWindow::GetLatLonfromINFO(QString Input,double &Lat, double &Lon) {
    bool        Find = false;
    int         j = 0;

    QStringList     list1 = Input.split(QRegularExpression("\n"));
    for ( j = 0 ; j < list1.length() ; j++) {
        if (list1.at(j).contains("Corner Coordinates")) {
            Find = true;
            j++;
            break;
        }
    }

    if (!Find) return (false);

    QStringList     list2 = list1.at(j).split(QRegularExpression("("));

    QRegularExpression regex("(?<=\\()[^)]*(?=\\))");
    QRegularExpressionMatchIterator matches = regex.globalMatch(list1.at(j));
    QStringList slstTerms;

    while( matches.hasNext() ) {
        QRegularExpressionMatch match = matches.next();

        if (match.hasMatch()) {
            slstTerms.append(match.captured(0));
        }
    }

    char    InputStr[100],Temp[100];
    int     i,k;
    double  Val;

    strcpy(InputStr,slstTerms.at(1).toStdString().c_str());

    i = 0;
    k = 0;

    do {
        if(isdigit(InputStr[i])) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != 'd');
    Temp[k] = 0;
    Val = atof(Temp);

    k = 0;
    do {
        if(isdigit(InputStr[i])) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != '\'');
    Temp[k] = 0;
    Val = Val + (atof(Temp) / 60.0);

    k = 0;
    do {
        if((isdigit(InputStr[i])) || (InputStr[i] == '.')) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != '\"');
    Temp[k] = 0;
    Val = Val + (atof(Temp) / 3600.0);
    if (InputStr[i+1] == 'W') Val = -Val;
    Lon = Val;

    k = 0;
    do {
        if(isdigit(InputStr[i])) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != 'd');
    Temp[k] = 0;
    Val = atof(Temp);

    k = 0;
    do {
        if(isdigit(InputStr[i])) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != '\'');
    Temp[k] = 0;
    Val = Val + (atof(Temp) / 60.0);

    k = 0;
    do {
        if((isdigit(InputStr[i])) || (InputStr[i] == '.')) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != '\"');
    Temp[k] = 0;
    Val = Val + (atof(Temp) / 3600.0);
    if (InputStr[i+1] == 'S') Val = -Val;
    Lat = Val;


/*
    while (!in.atEnd()) {
        QString line2;
        int Pos1,Pos2;
        line = in.readLine();
        if (line.contains(St1,Qt::CaseInsensitive)) { // Acha onde está a informação de cooredenas do polígono
        }
    }
*/

    return(true);
}


double MainWindow::getANGLEfromEXIF(GDALDataset *poSrcDS) {

    char            InputStr[100];
    const char      *Value;

    Value = poSrcDS->GetMetadataItem("DJI_YawDegree");

    if (Value == NULL) return (ERROR_ANGLE);

     strcpy(InputStr,Value);

     return (std::stod(InputStr));
}



double MainWindow::getLATfromEXIF(GDALDataset *poSrcDS) {
    char    InputStr[100],Temp[100];
    int     i,k;
    double  Val;

    i = 0;
    k = 0;
    strcpy(Temp,"");
    strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_GPSLatitude"));

    do {
        if(isdigit(InputStr[i])) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != ')');
    Temp[k] = 0;
    Val = atof(Temp);

    k = 0;
    do {
        if(isdigit(InputStr[i])) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != ')');
    Temp[k] = 0;
    Val = Val + (atof(Temp) / 60.0);

    k = 0;
    do {
        if((isdigit(InputStr[i])) || (InputStr[i] == '.')) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != ')');
    Temp[k] = 0;
    Val = Val + (atof(Temp) / 3600.0);

    strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_GPSLatitudeRef"));

    if (strcmp(InputStr,"S") == 0) Val = -Val;
    return(Val);
}

double MainWindow::getLONfromEXIF(GDALDataset *poSrcDS) {
    char    InputStr[100],Temp[100];
    int     i,k;
    double  Val;

    i = 0;
    k = 0;
    strcpy(Temp,"");
    strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_GPSLongitude"));

    do {
        if(isdigit(InputStr[i])) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != ')');
    Temp[k] = 0;
    Val = atof(Temp);

    k = 0;
    do {
        if(isdigit(InputStr[i])) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != ')');
    Temp[k] = 0;
    Val = Val + (atof(Temp) / 60.0);

    k = 0;
    do {
        if((isdigit(InputStr[i])) || (InputStr[i] == '.')) {
            Temp[k] = InputStr[i];
            k++;
        }
        i++;
    } while (InputStr[i] != ')');
    Temp[k] = 0;
    Val = Val + (atof(Temp) / 3600.0);

    strcpy(InputStr,poSrcDS->GetMetadataItem("EXIF_GPSLongitudeRef"));

    if (strcmp(InputStr,"W") == 0) Val = -Val;
    return(Val);
}

