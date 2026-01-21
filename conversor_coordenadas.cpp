#include "mainwindow.h"

#include "conversor_coordenadas.h"
#include "dialog_conv.h"

#include "LatLong-UTM.h"
#include "gdal_version.h"
#include "gdal_priv.h"
#include "ogr_spatialref.h"
#include "gdal_utils_priv.h"
#include <GeographicLib/GeoCoords.hpp>


void MainWindow::Inicializa(UTM_GEO &Val,GeographicLib::GeoCoords &Val_C)
{
    Val.Lat = 0;
    Val.Lon = 0;
    Val.Out = "";
    Val.Zone = 31;
    Val.Easting = 0;
    Val.Northing = 0;
    Val.MC = (6 * Val.Zone - 183);
    Val.Hemisferio = "";
    Val.Hemisferio_bool = false;
    Val.Northing_Signed = 0;

    Val_C.Reset(0,0);
}

void MainWindow::on_actionConversor_Coordenadas_triggered()
{
    UTM_GEO                     Valores;
    GeographicLib::GeoCoords    Valores_C;

    Inicializa(Valores,Valores_C);
    dialogConv = new Dialog_Conv(this);

    dialogConv->Inicializa(Valores,Valores_C);

    dialogConv->setWindowTitle("Conversor coordenadas");
    dialogConv->show();


}
