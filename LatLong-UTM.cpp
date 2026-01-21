// LatLong-UTM.c++
// Conversions:  LatLong to UTM;  and UTM to LatLong;
// by Eugene Reimer, ereimer@shaw.ca, 2002-December;
// with LLtoUTM & UTMtoLL routines based on those by Chuck Gantz chuck.gantz@globalstar.com;
// with ellipsoid & datum constants from Peter H Dana website (http://www.colorado.edu/geography/gcraft/notes/datum/edlist.html);
//
// Usage:  see the Usage() routine below;
//
// Copyright © 1995,2002,2010 Eugene Reimer, Peter Dana, Chuck Gantz.  Released under the GPL;  see http://www.gnu.org/licenses/gpl.html
// (Peter Dana's non-commercial clause precludes using the LGPL)


#define     PRECISAO    6

#include <QVector>

#include <cmath>			//2010-08-11: was <math.h>	  
#include <cstdio>			//2010-08-11: was <stdio.h>	  
#include <cstdlib>			//2010-08-11: was <stdlib.h>  
#include <cstring>			//2010-08-11: was <string.h>  
#include <cctype>			//2010-08-11: was <ctype.h>  
#include <iostream>			//2010-08-11: was <iostream.h>
#include <iomanip>			//2010-08-11: was <iomanip.h> 

#include <sstream>

#include "LatLong-UTM.h"

using namespace std;			//2010-08-11: added
const double PI       =	4*atan(1);	//Gantz used: PI=3.14159265;
const double deg2rad  =	PI/180;
const double rad2deg  =	180/PI;
const double k0       =	0.9996;


stringstream    out;

class Ellipsoid{
public:
	Ellipsoid(){};
    Ellipsoid(int id, QString nameQ, double radius, double fr){
        strcpy_s(Name,nameQ.toStdString().c_str());  EquatorialRadius=radius;  eccSquared=2/fr-1/(fr*fr);
	}
    char Name[100];
	double EquatorialRadius; 
	double eccSquared;
};

static Ellipsoid ellip[] = {		//converted from PeterDana website, by Eugene Reimer 2002dec
//		 eId,  Name,		   EquatorialRadius,    1/flattening;
    Ellipsoid( 0, "Airy1830",		6377563.396,	299.3249646),
	Ellipsoid( 1, "AiryModified",		6377340.189,	299.3249646),
	Ellipsoid( 2, "AustralianNational",	6378160,	298.25),
	Ellipsoid( 3, "Bessel1841Namibia",	6377483.865,	299.1528128),
	Ellipsoid( 4, "Bessel1841",		6377397.155,	299.1528128),
	Ellipsoid( 5, "Clarke1866",		6378206.4,	294.9786982),
	Ellipsoid( 6, "Clarke1880",		6378249.145,	293.465),
	Ellipsoid( 7, "EverestIndia1830",	6377276.345,	300.8017),
	Ellipsoid( 8, "EverestSabahSarawak",	6377298.556,	300.8017),
	Ellipsoid( 9, "EverestIndia1956",	6377301.243,	300.8017),
	Ellipsoid(10, "EverestMalaysia1969",	6377295.664,	300.8017),	//Dana has no datum that uses this ellipsoid!
	Ellipsoid(11, "EverestMalay_Sing",	6377304.063,	300.8017),
	Ellipsoid(12, "EverestPakistan",	6377309.613,	300.8017),
	Ellipsoid(13, "Fischer1960Modified",	6378155,	298.3),
	Ellipsoid(14, "Helmert1906",		6378200,	298.3),
	Ellipsoid(15, "Hough1960",		6378270,	297),
	Ellipsoid(16, "Indonesian1974",		6378160,	298.247),
	Ellipsoid(17, "International1924",	6378388,	297),
	Ellipsoid(18, "Krassovsky1940",		6378245,	298.3),
	Ellipsoid(19, "GRS80",			6378137,	298.257222101),
	Ellipsoid(20, "SouthAmerican1969",	6378160,	298.25),
	Ellipsoid(21, "WGS72",			6378135,	298.26),
	Ellipsoid(22, "WGS84",			6378137,	298.257223563)
};
#define	eClarke1866	5		//names for ellipsoidId's
#define	eGRS80		19
#define	eWGS72		21
#define	eWGS84		22

class Datum{
public:
	Datum(){};
    Datum(int id, QString Qname, int eid, double dx, double dy, double dz){
        strcpy_s(Name,Qname.toStdString().c_str());  eId=eid;  dX=dx;  dY=dy;  dZ=dz;
	}
    char Name[100];
	int   eId;
	double dX;
	double dY;
	double dZ;
};
static Datum datum[] = {		//converted from PeterDana website, by Eugene Reimer 2002dec
//	      Id,  Name,			eId,		dX,	dY,	dZ;	//when & where this datum is applicable
	Datum( 0, "NAD27_AK",			eClarke1866,	-5,	135,	172),	//NAD27 for Alaska Excluding Aleutians
	Datum( 1, "NAD27_AK_AleutiansE",	eClarke1866,	-2,	152,	149),	//NAD27 for Aleutians East of 180W
	Datum( 2, "NAD27_AK_AleutiansW",	eClarke1866,	2,	204,	105),	//NAD27 for Aleutians West of 180W
	Datum( 3, "NAD27_Bahamas",		eClarke1866,	-4,	154,	178),	//NAD27 for Bahamas Except SanSalvadorIsland
	Datum( 4, "NAD27_Bahamas_SanSalv",	eClarke1866,	1,	140,	165),	//NAD27 for Bahamas SanSalvadorIsland
	Datum( 5, "NAD27_AB_BC",		eClarke1866,	-7,	162,	188),	//NAD27 for Canada Alberta BritishColumbia
	Datum( 6, "NAD27_MB_ON",		eClarke1866,	-9,	157,	184),	//NAD27 for Canada Manitoba Ontario
	Datum( 7, "NAD27_NB_NL_NS_QC",		eClarke1866,	-22,	160,	190),	//NAD27 for Canada NewBrunswick Newfoundland NovaScotia Quebec
	Datum( 8, "NAD27_NT_SK",		eClarke1866,	4,	159,	188),	//NAD27 for Canada NorthwestTerritories Saskatchewan
	Datum( 9, "NAD27_YT",			eClarke1866,	-7,	139,	181),	//NAD27 for Canada Yukon
	Datum(10, "NAD27_CanalZone",		eClarke1866,	0,	125,	201),	//NAD27 for CanalZone (ER: is that Panama??)
	Datum(11, "NAD27_Cuba",			eClarke1866,	-9,	152,	178),	//NAD27 for Cuba
	Datum(12, "NAD27_Greenland",		eClarke1866,	11,	114,	195),	//NAD27 for Greenland (HayesPeninsula)
	Datum(13, "NAD27_Carribean",		eClarke1866,	-3,	142,	183),	//NAD27 for Antigua Barbados Barbuda Caicos Cuba DominicanRep GrandCayman Jamaica Turks
	Datum(14, "NAD27_CtrlAmerica",		eClarke1866,	0,	125,	194),	//NAD27 for Belize CostaRica ElSalvador Guatemala Honduras Nicaragua
	Datum(15, "NAD27_Canada",		eClarke1866,	-10,	158,	187),	//NAD27 for Canada
	Datum(16, "NAD27_ConUS",		eClarke1866,	-8,	160,	176),	//NAD27 for CONUS
	Datum(17, "NAD27_ConUS_East",		eClarke1866,	-9,	161,	179),	//NAD27 for CONUS East of Mississippi Including Louisiana Missouri Minnesota
	Datum(18, "NAD27_ConUS_West",		eClarke1866,	-8,	159,	175),	//NAD27 for CONUS West of Mississippi Excluding Louisiana Missouri Minnesota
	Datum(19, "NAD27_Mexico",		eClarke1866,	-12,	130,	190),	//NAD27 for Mexico
	Datum(20, "NAD83_AK",			eGRS80,		0,	0,	0),	//NAD83 for Alaska Excluding Aleutians
	Datum(21, "NAD83_AK_Aleutians",		eGRS80,		-2,	0,	4),	//NAD83 for Aleutians
	Datum(22, "NAD83_Canada",		eGRS80,		0,	0,	0),	//NAD83 for Canada
	Datum(23, "NAD83_ConUS",		eGRS80,		0,	0,	0),	//NAD83 for CONUS
	Datum(24, "NAD83_Hawaii",		eGRS80,		1,	1,	-1),	//NAD83 for Hawaii
	Datum(25, "NAD83_Mexico_CtrlAmerica",	eGRS80,		0,	0,	0),	//NAD83 for Mexico CentralAmerica
	Datum(26, "WGS72",			eWGS72,		0,	0,	0),	//WGS72 for world
	Datum(27, "WGS84",			eWGS84,		0,	0,	0)	//WGS84 for world
};
#define	dNAD27_MB_ON	6		//names for datumId's
#define	dNAD27_Canada	15
#define	dNAD83_Canada	22
#define	dNAD83_ConUS	23
#define	dWGS84		27


void DatumConvert(int dIn, double LatIn, double LongIn, double HtIn, int dTo,  double& LatTo, double& LongTo, double& HtTo){
   // converts LatLongHt in datum dIn, to LatLongHt in datum dTo;  2002dec: by Eugene Reimer, from PeterDana equations.
   // Lat and Long params are in degrees;  North latitudes and East longitudes are positive;  Height is in meters;
   // ==This approach to Datum-conversion is a waste of time;  to get acceptable accuracy a large table is needed -- see NADCON, NTv2...
   double a,ee,N,X,Y,Z,EE,p,b,t;
   
   //--transform to XYZ, using the "In" ellipsoid
   //LongIn += 180;
   a = ellip[datum[dIn].eId].EquatorialRadius;
   ee= ellip[datum[dIn].eId].eccSquared;
   N = a / sqrt(1 - ee*sin(LatIn*deg2rad)*sin(LatIn*deg2rad));
   X = (N + HtIn) * cos(LatIn*deg2rad) * cos(LongIn*deg2rad);
   Y = (N + HtIn) * cos(LatIn*deg2rad) * sin(LongIn*deg2rad);
   Z = (N*(1-ee) + HtIn) * sin(LatIn*deg2rad);

   //--apply delta-terms dX dY dZ
   //cout<<"\tX:" <<X <<" Y:" <<Y <<" Z:" <<Z;		//==DEBUG
   X+= datum[dIn].dX - datum[dTo].dX;
   Y+= datum[dIn].dY - datum[dTo].dY;
   Z+= datum[dIn].dZ - datum[dTo].dZ;
   //cout<<"\tX:" <<X <<" Y:" <<Y <<" Z:" <<Z;		//==DEBUG
   
   //--transform back to LatLongHeight, using the "To" ellipsoid
   a = ellip[datum[dTo].eId].EquatorialRadius;
   ee= ellip[datum[dTo].eId].eccSquared;
   EE= ee/(1-ee);
   p = sqrt(X*X + Y*Y);
   b = a*sqrt(1-ee);
   t = atan(Z*a/(p*b));
   LatTo = atan((Z+EE*b*sin(t)*sin(t)*sin(t)) / (p-ee*a*cos(t)*cos(t)*cos(t)));
   LongTo= atan(Y/X);
   HtTo  = p/cos(LatTo) - a/sqrt(1-ee*sin(LatTo)*sin(LatTo));
   LatTo  *= rad2deg;
   LongTo *= rad2deg;  LongTo -= 180;
}


void LLtoUTM(int eId, double Lat, double Long,  double& Northing, double& Easting, int& Zone){
   // converts LatLong to UTM coords;  3/22/95: by ChuckGantz chuck.gantz@globalstar.com, from USGS Bulletin 1532.
   // Lat and Long are in degrees;  North latitudes and East Longitudes are positive.
   double a = ellip[eId].EquatorialRadius;
   double ee= ellip[eId].eccSquared;
   Long -= int((Long+180)/360)*360;			//ensure longitude within -180.00..179.9
   double N, T, C, A, M;
   double LatRad = Lat*deg2rad;
   double LongRad = Long*deg2rad;

   Zone = int((Long + 186)/6);
   if( Lat >= 56.0 && Lat < 64.0 && Long >= 3.0 && Long < 12.0 )  Zone = 32;
   if( Lat >= 72.0 && Lat < 84.0 ){			//Special zones for Svalbard
      if(      Long >= 0.0  && Long <  9.0 )  Zone = 31;
      else if( Long >= 9.0  && Long < 21.0 )  Zone = 33;
      else if( Long >= 21.0 && Long < 33.0 )  Zone = 35;
      else if( Long >= 33.0 && Long < 42.0 )  Zone = 37;
   }
   double LongOrigin = Zone*6 - 183;			//origin in middle of zone
   double LongOriginRad = LongOrigin * deg2rad;

   double EE = ee/(1-ee);

   N = a/sqrt(1-ee*sin(LatRad)*sin(LatRad));
   T = tan(LatRad)*tan(LatRad);
   C = EE*cos(LatRad)*cos(LatRad);
   A = cos(LatRad)*(LongRad-LongOriginRad);

   M= a*((1 - ee/4    - 3*ee*ee/64 - 5*ee*ee*ee/256  ) *LatRad 
	    - (3*ee/8 + 3*ee*ee/32 + 45*ee*ee*ee/1024) *sin(2*LatRad)
	    + (15*ee*ee/256 + 45*ee*ee*ee/1024	  ) *sin(4*LatRad)
	    - (35*ee*ee*ee/3072			  ) *sin(6*LatRad));
   
   Easting = k0*N*(A+(1-T+C)*A*A*A/6+(5-18*T+T*T+72*C-58*EE)*A*A*A*A*A/120) + 500000.0;

   Northing = k0*(M+N*tan(LatRad)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24
			    + (61-58*T+T*T+600*C-330*EE)*A*A*A*A*A*A/720));
}


void UTMtoLL(int eId, double Northing, double Easting, int Zone,  double& Lat, double& Long){
   // converts UTM coords to LatLong;  3/22/95: by ChuckGantz chuck.gantz@globalstar.com, from USGS Bulletin 1532.
   // Lat and Long are in degrees;  North latitudes and East Longitudes are positive.
   double a = ellip[eId].EquatorialRadius;
   double ee = ellip[eId].eccSquared;
   double EE = ee/(1-ee);
   double e1 = (1-sqrt(1-ee))/(1+sqrt(1-ee));
   double N1, T1, C1, R1, D, M, mu, phi1Rad;
   double x = Easting - 500000.0;			//remove 500,000 meter offset for longitude
   double y = Northing;
   double LongOrigin = Zone*6 - 183;			//origin in middle of zone

   M = y / k0;
   mu = M/(a*(1-ee/4-3*ee*ee/64-5*ee*ee*ee/256));

   phi1Rad = mu + (3*e1/2-27*e1*e1*e1/32) *sin(2*mu) 
		+ (21*e1*e1/16-55*e1*e1*e1*e1/32) *sin(4*mu)
		+ (151*e1*e1*e1/96) *sin(6*mu);
   N1 = a/sqrt(1-ee*sin(phi1Rad)*sin(phi1Rad));
   T1 = tan(phi1Rad)*tan(phi1Rad);
   C1 = EE*cos(phi1Rad)*cos(phi1Rad);
   R1 = a*(1-ee)/pow(1-ee*sin(phi1Rad)*sin(phi1Rad), 1.5);
   D = x/(N1*k0);

   Lat = phi1Rad - (N1*tan(phi1Rad)/R1)*(D*D/2-(5+3*T1+10*C1-4*C1*C1-9*EE)*D*D*D*D/24
		   +(61+90*T1+298*C1+45*T1*T1-252*EE-3*C1*C1)*D*D*D*D*D*D/720);
   Lat *= rad2deg;
   Long = (D-(1+2*T1+C1)*D*D*D/6+(5-2*C1+28*T1-3*C1*C1+8*EE+24*T1*T1)*D*D*D*D*D/120) / cos(phi1Rad);
   Long = LongOrigin + Long*rad2deg;
}


//==procedures to use and test the LatLong-to-UTM and UTM-to-LatLong conversion routines

int Z,Fmt,K; double E,N,Lat,Long;		//global vars for Prt routine
#define fUT4	1				//for Fmt: UTM-4wd-style
#define fLLD	2				//for Fmt: LL-degrees-N|S-E|W
#define fLLDM	4				//for Fmt: LL-degrees:minutes-N|S-E|W
#define fLLDMS	8				//for Fmt: LL-degrees:minutes:seconds-N|S-E|W
#define fUTs	16				//for Fmt: UTM-signed
#define fLLs	32				//for Fmt: LL-degrees-signed
#define fUT3	64				//for Fmt: UTM-3wd-style
#define nbr	atof														//atof: ascii-to-number
#define Nbr	strtod
double  nbN(char*a){return strchr("Ss",*a) ?atof(a+1)-10000000 :strchr("Nn",*a) ?atof(a+1) :atof(a);}				//Northing
double  nbL(char*a){														//lat|long: [N|E|S|W]d[:m[:s]][N|E|S|W]
   char*p; double R=Nbr(a+!!isalpha(*a),&p); if(*p==':')R+=Nbr(p+1,&p)/60; if(*p==':')R+=Nbr(p+1,&p)/3600;
   return strchr("SsWw",*a)||(*p&&strchr("SsWw",*p)) ?-R :R;
}



void prtDM (double D){double M=(D-int(D))*60; cout<<setprecision(3)<<int(D)<<":"<<M;}
void prtDM (double D,QString &Out){
    double M=(D-int(D))*60; out<<setprecision(3)<<int(D)<<":"<<M;
//    Out = Out + QString::fromStdString(out.str());
}

void prtDMS(double D){double M=(D-int(D))*60; cout<<setprecision(1)<<int(D)<<":"<<int(M)<<":"<<(M-int(M))*60;}
void prtDMS(double D,QString &Out){
//    double M= (D - int(D)) * 60;
    float M,W;
    M = (D - int(D)) * 60.0;

    float S=(M-int(M))*60;
    out<<setprecision(3)<<int(D)<<":"<<int(M)<<":"<< S; ////////////////////////////////////////
//    Out = Out + QString::fromStdString(out.str());
}

void ps() {if(K++)cout<<" == ";}
void ps(QString &Out) {if(K++) out<<" == ";}


/*
void Prt(int Fmt, QString &Out) {
    K=0;
    out << setprecision(10) << setiosflags(ios::fixed);
    if(Fmt&fLLs)  {ps(Out); out<<setprecision(15) << Lat <<" " <<Long;								}	//LL degrees signed
//    if(Fmt&fLLs)  {ps(Out); out << Lat <<" " <<Long;								}	//LL degrees signed

//    if(Fmt&fLLD)  {ps(Out); char NS=(Lat<0?'S':'N'), EW=(Long<0?'W':'E'); out<<setprecision(5)<<abs(Lat)<<NS<<" "<<abs(Long)<<EW;	}	//LL degrees N|S E|W
    if(Fmt&fLLD)  {ps(Out); char NS=(Lat<0?'S':'N'), EW=(Long<0?'W':'E'); out<<abs(Lat)<<NS<<" "<<abs(Long)<<EW;	}	//LL degrees N|S E|W

    if(Fmt&fLLDM) {ps(Out); char NS=(Lat<0?'S':'N'), EW=(Long<0?'W':'E'); prtDM (abs(Lat),Out);out<<NS<<" ";prtDM (abs(Long),Out);out<<EW;}	//LL  d:m    N|S E|W
    if(Fmt&fLLDMS){ps(Out); char NS=(Lat<0?'S':'N'), EW=(Long<0?'W':'E'); prtDMS(abs(Lat),Out);out<<NS<<" ";prtDMS(abs(Long),Out);out<<EW;}	//LL  d:m:s  N|S E|W
    if(Fmt&fUTs)  {ps(Out); out<<setprecision(3)<<Z<<" "<<E<<" "<<N;								}	//UTM signed
    if(Fmt&fUT3)  {ps(Out); out<<setprecision(0)<<Z<<" "<<E<<" ";                  if(N<0)out<<'S'<<N+10000000;else out<<N;	}	//UTM std 3-word S-style
    if(Fmt&fUT4)  {ps(Out); out<<setprecision(0)<<Z<<" "<<(N<0?"S ":"N ")<<E<<" "; if(N<0)out     <<N+10000000;else out<<N;	}	//UTM std 4-word N|S-style

    Out = QString::fromStdString(out.str());
}
*/
void Prt(int Fmt, QString &Out) {
    K=0;
    out << setprecision(10) << setiosflags(ios::fixed);

    char NS=(Lat<0?'S':'N'), EW=(Long<0?'W':'E');
    prtDMS(abs(Lat),Out);
    out<<NS<<" ";
    prtDMS(abs(Long),Out);
    out<<EW;
    Out = QString::fromStdString(out.str());
}

void Prt(int Fmt){														//Prt: print according to Fmt bits
   K=0;
   if(Fmt&fLLs)  {ps(); cout<<setprecision(9) <<Lat <<" " <<Long;								}	//LL degrees signed  
   if(Fmt&fLLD)  {ps(); char NS=(Lat<0?'S':'N'), EW=(Long<0?'W':'E'); cout<<setprecision(5)<<abs(Lat)<<NS<<" "<<abs(Long)<<EW;	}	//LL degrees N|S E|W 
   if(Fmt&fLLDM) {ps(); char NS=(Lat<0?'S':'N'), EW=(Long<0?'W':'E'); prtDM (abs(Lat));cout<<NS<<" ";prtDM (abs(Long));cout<<EW;}	//LL  d:m    N|S E|W 
   if(Fmt&fLLDMS){ps(); char NS=(Lat<0?'S':'N'), EW=(Long<0?'W':'E'); prtDMS(abs(Lat));cout<<NS<<" ";prtDMS(abs(Long));cout<<EW;}	//LL  d:m:s  N|S E|W 
   if(Fmt&fUTs)  {ps(); cout<<setprecision(3)<<Z<<" "<<E<<" "<<N;								}	//UTM signed          
   if(Fmt&fUT3)  {ps(); cout<<setprecision(0)<<Z<<" "<<E<<" ";                  if(N<0)cout<<'S'<<N+10000000;else cout<<N;	}	//UTM std 3-word S-style
   if(Fmt&fUT4)  {ps(); cout<<setprecision(0)<<Z<<" "<<(N<0?"S ":"N ")<<E<<" "; if(N<0)cout     <<N+10000000;else cout<<N;	}	//UTM std 4-word N|S-style
}


void CvtLine(int C, char**V, QString &Out) {
    if     (C==2){Lat=nbL(V[0]); Long=nbL(V[1]);                                               LLtoUTM(eWGS84,Lat,Long,N,E,Z);}	//2 words, convert from LatLong
    else if(C==3){Z=atoi(V[0]); E=nbr(V[1]); N=nbN(V[2]);                                      UTMtoLL(eWGS84,N,E,Z,Lat,Long);}	//3 words, convert from UTM
    else         {Z=atoi(V[0]); E=nbr(V[2]); N=nbN(V[3]); N=strchr("Ss",V[1][0])?N-10000000:N; UTMtoLL(eWGS84,N,E,Z,Lat,Long);}	//4 words, convert from UTM 2nd wd is N|S
    Prt(Fmt,Out);											//print as per Fmt, add comment if any
}

void CvtLine(int C, char**V, char*p){
   if     (C==2){Lat=nbL(V[0]); Long=nbL(V[1]);                                               LLtoUTM(eWGS84,Lat,Long,N,E,Z);}	//2 words, convert from LatLong
   else if(C==3){Z=atoi(V[0]); E=nbr(V[1]); N=nbN(V[2]);                                      UTMtoLL(eWGS84,N,E,Z,Lat,Long);}	//3 words, convert from UTM
   else         {Z=atoi(V[0]); E=nbr(V[2]); N=nbN(V[3]); N=strchr("Ss",V[1][0])?N-10000000:N; UTMtoLL(eWGS84,N,E,Z,Lat,Long);}	//4 words, convert from UTM 2nd wd is N|S
   Prt(Fmt); if(*p)cout<<"\t"<<p; cout<<"\n";											//print as per Fmt, add comment if any
   //==Prt(Fmt) replaced by  Prt(fXXX);cout<<" == ";Prt(Fmt&~fXXX)  to show in approximately input-format first, then in the other formats?
}

void test(int eId, double pLat, double pLong){
   Lat=pLat; Long=pLong;
   cout<<"\nEllipsoid:" <<ellip[eId].Name;	cout<<"\nStarting Lat,Long position:              "; Prt(fLLs);
   LLtoUTM(eId, Lat, Long, N, E, Z);		cout<<"\nConverted to UTM-Zone,Easting,Northing:  "; Prt(fUTs);
   UTMtoLL(eId, N, E, Z, Lat, Long);		cout<<"\nConverted back to Lat,Long:              "; Prt(fLLs);
   cout<<"\n";
}
void testD(int dIn, double LatIn, double LongIn, double HtIn, int dTo){
   cout<<"\nDatum:" <<datum[dIn].Name <<" Ellipsoid:" <<ellip[datum[dIn].eId].Name  <<" To-Datum:" <<datum[dTo].Name <<" To-Ellipsoid:" <<ellip[datum[dTo].eId].Name;
   cout<<setprecision(9);							//note: Ht (in metres) needs 5-digits less precision than Lat,Long do
   double LatTo, LongTo, HtTo;					cout<<"\nIN: datum:" <<datum[dIn].Name <<"\tLat:" <<LatIn <<" Long:" <<LongIn <<" Ht:" <<HtIn;
   DatumConvert(dIn,LatIn,LongIn,HtIn, dTo,LatTo,LongTo,HtTo);	cout<<"\nTO: datum:" <<datum[dTo].Name <<"\tLat:" <<LatTo <<" Long:" <<LongTo <<" Ht:" <<HtTo;
   DatumConvert(dTo,LatTo,LongTo,HtTo, dIn,LatIn,LongIn,HtIn);	cout<<"\nTO: datum:" <<datum[dIn].Name <<"\tLat:" <<LatIn <<" Long:" <<LongIn <<" Ht:" <<HtIn;
   cout<<"\n";
}
void Testcases(){
   test(eWGS84,	  47+22.690/60, 8+13.950/60);					//WGS-84,     47:22.690N   8:13.950E    == ChuckGantz-eg  (Swiss Grid: 659879/247637)
   test(eWGS84,	  30+16./60+28.82/3600, -(97+44./60+25.19/3600));		//WGS-84,     30:16:28.82N 97:44:25.19W == PeterDana-eg1
   test(eClarke1866, 30+16./60+28.03/3600, -(97+44./60+24.09/3600));		//Clarke1866, 30:16:28.03N 97:44:24.09W == PeterDana-eg2;  recall NAD27 uses Clarke1866
   test(eClarke1866, 51.5, -101.5);						//Clarke1866, 51:30N 101:30W == SE-corner of topo-map:62N12
   testD(dWGS84, 30+16./60+28.82/3600, -(97+44./60+25.19/3600), 0, dWGS84);	//test DatumConvert on Dana-eg1 to itself (to see X,Y,Z)
   testD(dNAD27_MB_ON,  51.5, -101.5, 0, dNAD83_Canada);			//test DatumConvert on SE-corner of topo-map:62N12;  FROM: dNAD27_Canada | dNAD27_MB_ON
}
void Usage(){
   cout<<"Usage:";
   cout<<"\n	LatLong-UTM  [--Outputformat]  Latitude[N|S]  Longitude[E|W]";
   cout<<"\n	LatLong-UTM  [--Outputformat]  Zone  Easting  Northing";
   cout<<"\n	LatLong-UTM  [--Outputformat]  Zone  N|S  Easting  Northing";
   cout<<"\n	LatLong-UTM  [--Outputformat]";
   cout<<"\nreads from standard-input when no coordinates given on cmdline, where each line is one of:";
   cout<<"\n	Latitude[N|S]  Longitude[E|W]";
   cout<<"\n	Zone  Easting  Northing";
   cout<<"\n	Zone  N|S  Easting  Northing";
   cout<<"\nLatitude and Longitude are in degrees or degrees:minutes or degrees:minutes:seconds with north and east positive, S and W are alternate minus-signs;";
   cout<<"\nZone is an integer 1..60;  Easting in metres with origin for zone#N at 500,000m west of longitude N*6-183°;  northing is metres from equator with north positive;";
   cout<<"\nin the 3-word style, Northing is northing or the letter S followed by northing-plus-ten-million;";
   cout<<"\nin the 4-word style when 2nd word is S, Northing is northing-plus-ten-million;";
   cout<<"\nin the signed style, Northing is northing (and sanity prevails, unfortunately that style is not widely used);";
   cout<<"\nOutputformat is an integer, the sum of one or more of: 1 for UTM-4-word, 2 for LL-degrees, 4 for LL-degrees:minutes, 8 for LL-degrees:minutes:seconds,";
   cout<<"\n	16 for UTM-signed, 32 for LL-signed, 64 for UTM-3-word;  the default for Outputformat is 7 (which is UTM + LL-degrees + LL-degrees:minutes);";
   cout<<"\nan input-file can have comments, from '#' to end-of-line, and such comments as well as empty-lines will be echoed to the output;";
   cout<<"\nno datum conversion is provided, the WGS84 datum is used throughout (WGS84 is identical to GRS80 aka NAD83 to 8 significant digits);";
   cout<<"\nEXAMPLES:";
   cout<<"\n	LatLong-UTM  50.26     -96		-- convert Latitude:50.26 Longitude:-96 to UTM";
   cout<<"\n	LatLong-UTM  50:15.6   -96		-- same as preceding";
   cout<<"\n	LatLong-UTM  50:15:36  -96		-- same as preceding";
   cout<<"\n	LatLong-UTM  50:15:36N  96W		-- same as preceding";
   cout<<"\n	LatLong-UTM  N50:15:36  W96		-- same as preceding";
   cout<<"\n	LatLong-UTM  -25.0  18.13		-- convert Latitude:-25.0 Longitude:18.13 to UTM";
   cout<<"\n	LatLong-UTM  14 501000 5678901		-- convert Zone:14 Easting:501000 Northing:5678901 to LatLong";
   cout<<"\n	LatLong-UTM  14 501000 -5666777		-- convert Zone:14 Easting:501000 Northing:-5666777 to LatLong";
   cout<<"\n	LatLong-UTM  14 501000 S4333223		-- same as preceding;  Northing as S followed by Northing-plus-ten-million";
   cout<<"\n	LatLong-UTM  14 S 501000 4333223	-- same as preceding, in the 4-word style";
   cout<<"\n	LatLong-UTM  --48  <trailpoints-utm.txt	-- convert each line of trailpoints-utm.txt, with Outputformat: UTM-signed + LL-signed";
   cout<<"\n";
}


//int conv(int argc, char**argv,QString &Out,double &Latitude,double &Longitude,double &North, double &East, double *Zone) {
int conv(int argc, char**argv,UTM_GEO &Geo){
    //2010-08-11: was void main(...
   char buf[256]; char*p; int L,ac; char*av[129];				//vars for reading stdin
   cout<<setiosflags(ios::fixed);						//decided against including ios::showpoint
   Fmt=fUT4|fLLD|fLLDM;								//default for Fmt, if not specified by input
   --argc; ++argv;								//remove spurious first element of argv array

   out.str(std::string());

   while(argc && memcmp(argv[0],"--",2)==0){					//handle leading options: --Outputformat, --test, --help
      if     (isdigit(argv[0][2]))		Fmt=atoi(argv[0]+2);		//for --<DIGIT>, parse Outputformat into Fmt
      else if(strcmp(argv[0],"--test")==0)	{Testcases(); return 0;}	//for --test, run testcases & exit
      else					{Usage(); return 0;}		//for --help, show Usage & exit
      --argc; ++argv;
   }
   if(argc==0)  while(cin.getline(buf,256), cin.good()){			//0 args, read stdin converting each line
      p=buf; ac=0;
      while(1){ while(*p&&strchr(" \t",*p))++p; if(*p==0||*p=='#')break; av[ac++]=p; while(*p&&!strchr(" \t#",*p))++p;}	//break line into whitespace-separated words
      if(ac>=2&&ac<=4) CvtLine(ac,av,p);					//line with 2|3|4 words, convert and print
      else if(ac==0)   cout<<buf<<"\n";						//line with no words, echo the line (comments)
      else             cout<<"==invalid number-of-words: "<<buf<<"\n";		//anything else is invalid, produce errmsg
   } else if(argc>=2&&argc<=4)	{
       out.clear();
       Geo.Out = "";
       CvtLine(argc,argv,Geo.Out);
       cout << setprecision(15) << Geo.Out.toStdString();
       cout << "\n" ;
//       CvtLine(argc,argv,NULL); //2|3|4 args, convert and print
   } else {
       Usage();					//argc other than 0|2|3 is invalid, show Usage
   }

   Geo.Lat = Lat;
   Geo.Lon = Long;
   Geo.Northing_Signed = N;
   Geo.Northing = N < 0 ? N+10000000 : N;
   Geo.Hemisferio_bool = Lat < 0 ? false : true;
   Geo.Hemisferio = Lat < 0 ? 'S' : 'N';
   Geo.Easting = E;
   Geo.Zone = Z;

   return 0;									//2010-08-11: added when void became illegal
}

int conv(int argc, char**argv){
    QString Out;

    //2010-08-11: was void main(...
   char buf[256]; char*p; int L,ac; char*av[129];				//vars for reading stdin
   cout<<setiosflags(ios::fixed);						//decided against including ios::showpoint
   Fmt=fUT4|fLLD|fLLDM;								//default for Fmt, if not specified by input
   --argc; ++argv;								//remove spurious first element of argv array
   while(argc && memcmp(argv[0],"--",2)==0){					//handle leading options: --Outputformat, --test, --help
      if     (isdigit(argv[0][2]))		Fmt=atoi(argv[0]+2);		//for --<DIGIT>, parse Outputformat into Fmt
      else if(strcmp(argv[0],"--test")==0)	{Testcases(); return 0;}	//for --test, run testcases & exit
      else					{Usage(); return 0;}		//for --help, show Usage & exit
      --argc; ++argv;
   }
   if(argc==0)  while(cin.getline(buf,256), cin.good()){			//0 args, read stdin converting each line
      p=buf; ac=0;
      while(1){ while(*p&&strchr(" \t",*p))++p; if(*p==0||*p=='#')break; av[ac++]=p; while(*p&&!strchr(" \t#",*p))++p;}	//break line into whitespace-separated words
      if(ac>=2&&ac<=4) CvtLine(ac,av,p);					//line with 2|3|4 words, convert and print
      else if(ac==0)   cout<<buf<<"\n";						//line with no words, echo the line (comments)
      else             cout<<"==invalid number-of-words: "<<buf<<"\n";		//anything else is invalid, produce errmsg
   } else if(argc>=2&&argc<=4)	{
       out.clear();
       Out = "";
       CvtLine(argc,argv,Out);
       cout << setprecision(15) << Out.toStdString();
       cout << "\n" ;
//       CvtLine(argc,argv,NULL); //2|3|4 args, convert and print
   } else {
       Usage();					//argc other than 0|2|3 is invalid, show Usage
   }

   return 0;									//2010-08-11: added when void became illegal
}


// 2002-December:  by Eugene Reimer, ereimer@shaw.ca
// started with 2 routines (LLtoUTM, UTMtoLL) by Chuck Gantz chuck.gantz@globalstar.com;
//	Gantz cites his source as:  USGS Bulletin#1532;
// dropped the "LatZone" letters (aka Latitude-Bands);
// reworked handling "origin 10million" for northings in Southern-hemisphere -- his method needed "LatZone" letters which are even sillier than ten-million convention;
// better value for PI:
//	made 6mm difference in calculated UTM coords (in Gantz example);
// ellipsoid constants:  replaced Gantz's constants, which he said were from PeterDana website, with data from PeterDana website (to my surprise they differ):
//	http://www.colorado.edu/geography/gcraft/notes/datum/edlist.html
//	Dana cites his sources as:  NIMA-8350.2-1977july4  and  MADTRAN-1996october1
//	-- better Clarke1866 ellipsoid made 360mm difference in calculated UTM coords in MB;  NAD-27 example in Manitoba now within 1mm of NR-Canada online-converter!!
//	-- computed UTM-coords for AustinDomeStar differ from Dana's by roughly 100mm; but agree to the mm with NR-Canada online-converter (nrcan.gc.ca)!!
// datum constants & datum conversion routine:  using formulae from PeterDana website;  the dX,dY,dZ constants are for the conversion formulae;
//	have defined only those datums that apply to NorthAmerica (28 out of several hundred);
//	Dana cites:  Bowring,B. 1976. Transformations from spatial to geographical coordinates. Survey Review XXIII; pg323-327.
// added support for input from a file;  am tempted to scrap cmdline-args in which case the Usage-Examples would become:  echo "..." | LatLong-UTM
// considered scrapping Testcases routine and using a bash-script of testcases instead;  ==keeping it for non-WGS84 testcases since my UI has no datum-conversion...

// 2010-08-11:  in 2002 this compiled just fine,  but the current g++ insists main must return int;  and warns about deprecated header-files;
//	fixed by revising void main() --> int main();  adding return 0;  revising #include's as shown above eg: <math.h> --> <cmath>;  adding: using namespace std;

// 2010-08-12:  debugging DatumConvert:
//	my 1st testD eg, Dana-eg1, going TO-XYZ;
//		according to Dana it is:	X:-742507.1    Y:-5462738.5    Z:3196706.5		(Dana calls it ECEF XYZ)
//		I was getting:			X:-743130.2424 Y:-5467322.9359 Z:3199389.2544
//		I'm now getting (sin-sqred):	X:-742507.1145 Y:-5462738.4892 Z:3196706.5101		<==AGREES after sin->sin-sqred fix!!
//		GEOTRANS (to Geocentric) gets:	X:-742507      Y:-5462738      Z:3196707		(Geotrans calls it Geocentric)
//		ergo: GEOTRANS agrees with Dana, my code based on Dana-equations does too after fixing;
//	my 2nd testD eg, converting Lat:51.5 Long:101.5 from NAD27 to NAD83:
//		NTv2 (online):		Lat:51:29:59.96845  Long:101:30:1.65354W			<==using http://www.geod.nrcan.gc.ca/apps/ntv2/ntv2_geo_e.php
//					Lat:51.49999124     Long:101.5004593W				<--converted to degrees on my calculator (10-digit precision)
//		NTv2 (exe under Wine):  Lat:51.499991236    Long:101.500459317W				<==AUTHORITATIVE ANSWER
//		NADCON gets:		Lat:==NO LUCK, NORTH-AMERICA==USA==!!==				<==http://www.ngs.noaa.gov/cgi-bin/nadcon.prl
//		GEOTRANS gets:		Lat:51.50004        Long:-101.50059      Ht:-25			<--inaccurate;  its UI truncates Datum-name => unsure which??
//		I'm getting:		Lat:51.500036202    Long:-101.500594748  Ht:-24.919516822	<--From:NAD27_Canada;  same as GEOTRANS but not good enough
//		I'm getting:		Lat:51.500013926    Long:-101.500577765  Ht:-26.781441874    	<--From:NAD27_MB_ON;   slightly closer but still not good enough
//		Paper http://www.h-e.com/pdfs/de_sbe94.pdf:  shows MB has approx no change in Lat,  from -1.0 in E-MB to -1.6seconds in W-MB change in Long;
//			same info in degrees:  Longitude from -0.00027 in Eastern-MB to -0.00044degrees in Western-MB;  <--my eg in W-MB changes by -0.00046degrees;
//			Error on that map-corner-eg to 5-place-fractional-degrees:  Lat-error: 0.00001  Long-error: 00012;
//	==Conclusions after some reading:  (1) NADCON & NTv2(NRCAN) are the reasonably accurate methods;  (2) my (and GEOTRANS's) formula approach is a waste of time;
//	PROJ (http://trac.osgeo.org/proj/) -- it needs grid shift file, eg: NTV2_0.GSB from http://www.geod.nrcan.gc.ca/online_data_e.php
//	NTv2, DOS-program that comes with the "grid shift file" NTV2_0.GSB from NRCAN, runs under Wine;  http://www.geod.nrcan.gc.ca/online_data_e.php
//	note: NTv2 is Canadian;  Australia, NewZealand, Germany, Switzerland, Spain also use its file-format;  and unofficially GreatBritain, France, Portugal;
//	NADCON is yank;  they call it the NORTH-AMERICAN converter but it only supports locations within-USA:-)
//	TRANSDAT: Windows program installed via Wine, into c:\Program Files\transdat;  	needs MFC42.DLL, installed into /home/ereimer/.wine/drive_c/windows/system32
// NTv2:  have found OpenSource programs with NTv2 support;  also found freely downloadable NTv2 Data-files;  and the NTv2 Developer's Guide;
// Notes on NTv2 doc & tools are now in my Datum-Conversion rant:  http://ereimer.net/rants/datum-conversion.htm
//	[For me: copies of NTv2 doc & tools are in:  /pix/er-GIS-DatumConversion-NAD83-WGS84-NTv2-etc-data;  see 00-NOTES... in that dir]
// those tools offer UTM<->LatLong as well as Datum-shift conversions;  may abandon this program as hardly needed, except for more flexible outputformat==??==
// one thing that is needed:  a NAD27->NAD83 conversion-table (in NTv2 format) that handles all of North America==??==

// 2010-08-30:
// put up a web-interface:  http://ereimer.net/cgi-bin/coordinatecvt;
// nbL:  accept N|S E|W at either end of a  Lat/Long coordinate;  (letter-up-front style is not offered for output);
// CvtLine:  accept UTM-4-word input eg: 14 N 501000 5678901 (same as 14 501000 N5678901);  Prt: fUT4 is Fmt bit for UTM-4-word output, replaces fUT3 as on-by-default;
// Usage: added Examples and reworded Instructions for the 4-word UTM style now supported on input & output;
// Consider:
//	doing space->colon fixups to get Lat Long pair might be possible, since valid UTM-Easting is never less than 60...
//	adding some checking for better errmsgs;  eg: each "word" must start with digit|minus-sign|N|S|E|W|n|s|e|w;
