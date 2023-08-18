#ifndef GDAL_HAM_H
#define GDAL_HAM_H

#include <opencv2/core/core.hpp>
#include <QString>
#include <gdal.h>
#include "gdal_priv.h"

//#include "cpl_string.h"
//#include "gdal_version.h"
//#include "ogr_spatialref.h"
//#include "commonutils.h"
//#include "gdal_utils_priv.h"

#include "geo_exif_xmp.h"

void Grava_VRT_from_file(QString,QString);
void GDAL2MAT(GDALDataset *,cv::Mat &);
void MAT2GDAL(QString output,cv::Mat output_image,char **papszMetadata,double *Transform);
//void writeGeoInfo(QString output,cv::Mat output_image,char **papszMetadata,Geometria_Imagem &Geometria_ImgProc);
void GDAL2PIX(QString FileName,QPixmap pix);
QPixmap MAT2PIX(QString FileName);
static QImage MatToQImage(const cv::Mat& mat);

#endif // GDAL_HAM_H
