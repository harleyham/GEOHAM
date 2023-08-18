#ifndef GEO_EXIF_XMP_H
#define GEO_EXIF_XMP_H

#include <opencv2/core.hpp>

#include <GeographicLib/GeoCoords.hpp>

#include <QString>
#include <QStringList>
#include <QFileInfoList>

#include <gdal.h>
#include "gdal_priv.h"

#include "Windows.h"


#include <complex>

//
// Class responsible for storing and parsing EXIF information from a JPEG blob
//

/*
class TESTE {

};
*/

class EXIF_XMP_Info {
 public:
  // Parsing function for an entire JPEG image buffer.
  //
  // PARAM 'data': A pointer to a JPEG image.
  // PARAM 'length': The length of the JPEG image.
  // RETURN:  PARSE_EXIF_SUCCESS (0) on succes with 'result' filled out
  //          error code otherwise, as defined by the PARSE_EXIF_ERROR_* macros
  int parseFrom(const unsigned char *data, unsigned length);
  int parseFrom(const std::string &data);

  // Parsing function for an EXIF segment. This is used internally by parseFrom()
  // but can be called for special cases where only the EXIF section is
  // available (i.e., a blob starting with the bytes "Exif\0\0").
  int parseFromEXIFSegment(const unsigned char *buf, unsigned len);

  // Set all data members to default values.
  void clear();

  // Data fields filled out by parseFrom()
  char ByteAlign;                   // 0 = Motorola byte alignment, 1 = Intel
  std::string ImageDescription;     // Image description
  std::string Make;                 // Camera manufacturer's name
  std::string Model;                // Camera model
  unsigned short Orientation;       // Image orientation, start of data corresponds to
                                    // 0: unspecified in EXIF data
                                    // 1: upper left of image
                                    // 3: lower right of image
                                    // 6: upper right of image
                                    // 8: lower left of image
                                    // 9: undefined
  unsigned short BitsPerSample;     // Number of bits per component
  std::string Software;             // Software used
  std::string DateTime;             // File change date and time
  std::string DateTimeOriginal;     // Original file date and time (may not exist)
  std::string DateTimeDigitized;    // Digitization date and time (may not exist)
  std::string SubSecTimeOriginal;   // Sub-second time that original picture was taken
  std::string Copyright;            // File copyright information
  double ExposureTime;              // Exposure time in seconds
  double FNumber;                   // F/stop
  unsigned short ExposureProgram;   // Exposure program
                                    // 0: Not defined
                                    // 1: Manual
                                    // 2: Normal program
                                    // 3: Aperture priority
                                    // 4: Shutter priority
                                    // 5: Creative program
                                    // 6: Action program
                                    // 7: Portrait mode
                                    // 8: Landscape mode
  unsigned short ISOSpeedRatings;   // ISO speed
  double ShutterSpeedValue;         // Shutter speed (reciprocal of exposure time)
  double ExposureBiasValue;         // Exposure bias value in EV
  double SubjectDistance;           // Distance to focus point in meters
  double FocalLength;               // Focal length of lens in millimeters
  unsigned short FocalLengthIn35mm; // Focal length in 35mm film
  char Flash;                       // 0 = no flash, 1 = flash used
  unsigned short FlashReturnedLight;// Flash returned light status
                                    // 0: No strobe return detection function
                                    // 1: Reserved
                                    // 2: Strobe return light not detected
                                    // 3: Strobe return light detected
  unsigned short FlashMode;         // Flash mode
                                    // 0: Unknown
                                    // 1: Compulsory flash firing
                                    // 2: Compulsory flash suppression
                                    // 3: Automatic mode
  unsigned short MeteringMode;      // Metering mode
                                    // 1: average
                                    // 2: center weighted average
                                    // 3: spot
                                    // 4: multi-spot
                                    // 5: multi-segment
  unsigned ImageWidth;              // Image width reported in EXIF data
  unsigned ImageHeight;             // Image height reported in EXIF data

  struct Geolocation_t {            // GPS information embedded in file
    double Latitude;                  // Image latitude expressed as decimal
    double Longitude;                 // Image longitude expressed as decimal
    double Altitude;                  // Altitude in meters, relative to sea level
    double Altitude_Rel;              // Altitude im meters, relative start
    char AltitudeRef;                 // 0 = above sea level, -1 = below sea level
    double DOP;                       // GPS degree of precision (DOP)
    double Direction_Gimbal;          // Direction of the gimbal in degrees
    struct Coord_t {
      double degrees;
      double minutes;
      double seconds;
      char direction;
    } LatComponents, LonComponents;   // Latitude, Longitude expressed in deg/min/sec
  } GeoLocation;

  struct LensInfo_t {               // Lens information
    double FStopMin;                // Min aperture (f-stop)
    double FStopMax;                // Max aperture (f-stop)
    double FocalLengthMin;          // Min focal length (mm)
    double FocalLengthMax;          // Max focal length (mm)
    double FocalPlaneXResolution;   // Focal plane X-resolution
    double FocalPlaneYResolution;   // Focal plane Y-resolution
    unsigned short FocalPlaneResolutionUnit; // Focal plane resolution unit
                                             // 1: No absolute unit of measurement.
                                             // 2: Inch.
                                             // 3: Centimeter.
                                             // 4: Millimeter.
                                             // 5: Micrometer.
    std::string Make;               // Lens manufacturer
    std::string Model;              // Lens model
  } LensInfo;


  EXIF_XMP_Info() {
      clear();
  }

};


class TiposEXIF {

public:
    QStringList myOptions;


    TiposEXIF() {
        myOptions   << "Exif.Image.ImageDescription"
                    << "Exif.Image.Make"
                    << "Exif.Image.Model"
                    << "Exif.GPSInfo.GPSLatitudeRef"
                    << "Exif.GPSInfo.GPSLatitude"
                    << "Exif.GPSInfo.GPSLongitudeRef"
                    << "Exif.GPSInfo.GPSLongitude"
                    << "Exif.GPSInfo.GPSAltitude"
                    << "Exif.Photo.FocalLengthIn35mmFilm"
                    << "Exif.Photo.FocalLength"
                    << "Exif.Photo.PixelXDimension"
                    << "Exif.Photo.PixelYDimension";
        }
};

class TiposXMP {
public:
    QStringList myOptions;

    TiposXMP() {
        myOptions << "Xmp.drone-dji.AbsoluteAltitude"
                  << "Xmp.drone-dji.RelativeAltitude"
                  << "Xmp.drone-dji.GimbalRollDegree"
                  << "Xmp.drone-dji.GimbalYawDegre"
                  << "Xmp.drone-dji.GimbalPitchDegree"
                  << "Xmp.drone-dji.FlightRollDegree"
                  << "Xmp.drone-dji.FlightYawDegre"
                  << "Xmp.drone-dji.FlightPitchDegree";
    }
};


class TiposDrones {
public:
    QStringList myOptions;

    TiposDrones() {
        myOptions << "Inspire 1"
                  << "Mavic 1 Pro"
                  << "Mavic 2 Pro"
                  << "Matrice com X4S"
                  << "Matrice com Z30"
                  << "Mavic Mini"
                  << "Mavic 3"
                  << "Matrice 300";

    }
};

class TiposCameras {
public:
    QStringList myOptions;

    TiposCameras() {
        myOptions << "FC350"
                  << "FC220"
                  << "L1D-20c"
                  << "FC6510"
                  << "ZENMUSE Z30"
                  << "FC7203"
                  << "L2D-20c"
                  << "ZH20T";

    }
};


class Geometria_Imagem {
public:
    QString             Name;
    QFileInfo           Complete_Name;
    double              Iw;                     // Dimensão horizontal - Weigth - número de colunas
    double              Ih;                     // Dimensão vertical - Heigth - número de linhas
    std::complex<double> Cc;                    // Coordenadas LAT/Lon do Centro da Imagem
    std::complex<double> A,B,C,D;               // Coordenadas LAT/LON originais
    std::complex<double> Cc_UTM;                    // Coordenadas LAT/Lon do Centro da Imagem
    std::complex<double> A_UTM,B_UTM,C_UTM,D_UTM;   // Coordenadas LAT/LON originais
    double              Alt = 0;                                     // Altutude da foto em relação ao ponto de decolagem (metros)
    double              Alt_Default;
    double              Gimbal_Angle;            // Angulo do Gimbal em relação ao Norte no momento da foto
    int                 DimX,DimY;               // Dimensões da imagem original;
    double              DistFocal;               // Em milimetros
    double              DistFocal35mm;
    double              Sw;                      // Em milimetros
    double              GSD;                     // em centimetros/pixel (
//    double              GSDy;                    // em cm/px na dimensão y (altura da imagem)
    double              Dw,Dh;                   // Tamanho da imagem en relação ao terreno (metros)

    double              dx,dy;                   // Metade da imagem (metros)

    double              mppx,mppy;               // metros por pixel
    double              x_Size,y_Size;
    double              cmppx,cmppy;             // centimetros por pixel
    double              T1,T5;                   // Passo LON, Passo LAT

    double              Area;                    // Área em metros quadrados

    GeographicLib::GeoCoords CcUTM;
    GeographicLib::GeoCoords AUTM;
    GeographicLib::GeoCoords BUTM;
    GeographicLib::GeoCoords CUTM;
    GeographicLib::GeoCoords DUTM;

    GDALDataset         *FileDS;

    QList< std::complex<double> >   Pontos;
    QList< std::complex<int> >      PImg;

    cv::Mat             image;
    cv::Mat             image_Lida;

    QString             ExifXMPInfo;

    double              Escala;

    QString             TipoCamera = "";
    QString             TipoDrone = "";

    bool                Circle = true;
    bool                Line = false;
    bool                Poligono = false;
    bool                Preenche = true;
    bool                Proximidade = false;
    int                 Proximidade_Num;
    bool                Era_Proximo = false;
    bool                Selecionado = false;

    bool                PlotCenter = false;

    int                 Cor_Nodata;

    HWND                hParent;
    HWND                hWnd;

    bool                Blend_Utilizado = false;

    bool                CoordCentrais = false;

    bool                DeslocaImagem = false;

    bool                AjustaImagens = false;


    void Calculos(void);

    void CopyTo(Geometria_Imagem &Dest, cv::Mat Image);

    Geometria_Imagem() {
        Escala = 1.0;
    }
};

class Imagem_Geo {
public:
    QString                         Nome;
    QFileInfo                       Complete_Name;
//    GeographicLib::GeoCoords        CcUTM;
//    GeographicLib::GeoCoords        AUTM;
//    GeographicLib::GeoCoords        DUTM;
//    GDALDataset                     *FileDS;
//    cv::Mat                         Image;
//    double                          mppx,mppy;
//    double                          Iw,Ih;
    Geometria_Imagem                Geo;

};

class Imagem_Total {
public:
    QString             Nome,Make,Model;
    QFileInfoList       Geo_File_List;
    Geometria_Imagem    Img_Total;
    QFileInfo           Complete_Name;
    bool                Primeira_Ultima_Linha;
    int                 Sentido,Num;
    double              Media_Ida,Media_Volta,Altura,Lat,Lon;
};

int XMP_Print(char**);

int EXIF_Print(int,char**);

//void Rotaciona(cv::Mat, cv::Mat &, double);
void Rotaciona2(cv::Mat, cv::Mat &, double);

#endif // GEO_EXIF_XMP_H
