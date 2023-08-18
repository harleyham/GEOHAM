#ifndef LATLONGUTM_H
#define LATLONGUTM_H

#include <QString>

class UTM_GEO {

public:
    double      Lat,Lon;
    double      Northing,Easting,Northing_Signed;
    int         Zone,MC;
    QString     Out;
    bool        Hemisferio_bool;
    QString     Hemisferio;
};

int conv(int argc, char**argv);
int conv(int argc, char**argv,UTM_GEO &Geo);


#endif // LATLONGUTM_H
