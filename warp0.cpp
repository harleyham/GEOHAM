/******************************************************************************
 *
 * Project:  High Performance Image Reprojector
 * Purpose:  Test program for high performance warper API.
 * Author:   Frank Warmerdam <warmerdam@pobox.com>
 *
 ******************************************************************************
 * Copyright (c) 2002, i3 - information integration and imaging
 *                          Fort Collin, CO
 * Copyright (c) 2007-2015, Even Rouault <even dot rouault at spatialys.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

//#define MEFU 1

#define GDAL_COMPILATION 1


#include "cpl_string.h"
#include "cpl_error_internal.h"
#include "gdal_version.h"
#include "commonutils.h"
#include "gdal_utils_priv.h"


//#include "mainwindow.h"

#include <vector>

#include "warp.h"

CPL_CVSID("$Id$")

#ifdef MEFU
extern message_function MeFu;
extern Ui::MainWindow* windowPrincipal;
#endif


int warp_H(int, char **);

/************************************************************************/
/*                               GDALExit()                             */
/*  This function exits and cleans up GDAL and OGR resources            */
/*  Perhaps it should be added to C api and used in all apps?           */
/************************************************************************/

static int GDALExit( int nCode )
{
  const char  *pszDebug = CPLGetConfigOption("CPL_DEBUG",nullptr);
  if( pszDebug && (EQUAL(pszDebug,"ON") || EQUAL(pszDebug,"") ) )
  {
    GDALDumpOpenDatasets( stderr );
    CPLDumpSharedList( nullptr );
  }

  GDALDestroyDriverManager();

  OGRCleanupAll();

  exit( nCode );
}

/************************************************************************/
/*                               Usage()                                */
/************************************************************************/

static void Usage(const char* pszErrorMsg = nullptr)
{
    printf(
        "Usage: gdalwarp [--help-general] [--formats]\n"
        "    [-s_srs srs_def] [-t_srs srs_def] [-to \"NAME=VALUE\"]* [-novshiftgrid]\n"
        "    [-order n | -tps | -rpc | -geoloc] [-et err_threshold]\n"
        "    [-refine_gcps tolerance [minimum_gcps]]\n"
        "    [-te xmin ymin xmax ymax] [-tr xres yres] [-tap] [-ts width height]\n"
        "    [-ovr level|AUTO|AUTO-n|NONE] [-wo \"NAME=VALUE\"] [-ot Byte/Int16/...] [-wt Byte/Int16]\n"
        "    [-srcnodata \"value [value...]\"] [-dstnodata \"value [value...]\"] -dstalpha\n"
        "    [-r resampling_method] [-wm memory_in_mb] [-multi] [-q]\n"
        "    [-cutline datasource] [-cl layer] [-cwhere expression]\n"
        "    [-csql statement] [-cblend dist_in_pixels] [-crop_to_cutline]\n"
        "    [-of format] [-co \"NAME=VALUE\"]* [-overwrite]\n"
        "    [-nomd] [-cvmd meta_conflict_value] [-setci] [-oo NAME=VALUE]*\n"
        "    [-doo NAME=VALUE]*\n"
        "    srcfile* dstfile\n"
        "\n"
        "Available resampling methods:\n"
        "    near (default), bilinear, cubic, cubicspline, lanczos, average, mode,  max, min, med, Q1, Q3, sum.\n" );

    if( pszErrorMsg != nullptr )
        fprintf(stderr, "\nFAILURE: %s\n", pszErrorMsg);

    GDALExit( 1 );
}

/************************************************************************/
/*                       GDALWarpAppOptionsForBinaryNew()             */
/************************************************************************/

static GDALWarpAppOptionsForBinary *GDALWarpAppOptionsForBinaryNew(void)
{
    return static_cast<GDALWarpAppOptionsForBinary*>(CPLCalloc(  1, sizeof(GDALWarpAppOptionsForBinary) ));
}

/************************************************************************/
/*                       GDALWarpAppOptionsForBinaryFree()            */
/************************************************************************/

static void GDALWarpAppOptionsForBinaryFree( GDALWarpAppOptionsForBinary* psOptionsForBinary )
{
    if( psOptionsForBinary )
    {
        CSLDestroy(psOptionsForBinary->papszSrcFiles);
        CPLFree(psOptionsForBinary->pszDstFilename);
        CSLDestroy(psOptionsForBinary->papszOpenOptions);
        CSLDestroy(psOptionsForBinary->papszDestOpenOptions);
        CSLDestroy(psOptionsForBinary->papszCreateOptions);
        CPLFree(psOptionsForBinary);
    }
}

/************************************************************************/
/*                          WarpTermProgress()                          */
/************************************************************************/

static int gnSrcCount = 0;

static int CPL_STDCALL WarpTermProgress( double dfProgress,
                                         const char * pszMessage,
                                         void*)
{
    static CPLString osLastMsg;
    static int iSrc = -1;
    if( pszMessage == nullptr )
    {
        iSrc = 0;
    }
    else if( pszMessage != osLastMsg )
    {
        printf("%s : ", pszMessage);
        osLastMsg = pszMessage;
        iSrc ++;
    }
    return GDALTermProgress(dfProgress * gnSrcCount - iSrc, nullptr, nullptr);
}

/************************************************************************/
/*                                main()                                */
/************************************************************************/

//MAIN_START(argc, argv)
int warp_H(int argc, char **argv)
{
    GDALDatasetH *pahSrcDS = nullptr;
    int nSrcCount = 0;

    EarlySetConfigOptions(argc, argv);

/* -------------------------------------------------------------------- */
/*      Register standard GDAL drivers, and process generic GDAL        */
/*      command options.                                                */
/* -------------------------------------------------------------------- */
    GDALAllRegister();
    argc = GDALGeneralCmdLineProcessor( argc, &argv, 0 );
    if( argc < 1 )
        GDALExit( -argc );

#ifdef MEFU
    MeFu.Print(windowPrincipal," 1 ");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif

    for( int i = 0; argv != nullptr && argv[i] != nullptr; i++ )
    {
        if( EQUAL(argv[i], "--utility_version") )
        {
            printf("%s was compiled against GDAL %s and is running against GDAL %s\n",
                   argv[0], GDAL_RELEASE_NAME, GDALVersionInfo("RELEASE_NAME"));
            CSLDestroy( argv );
            return 0;
        }
        else if( EQUAL(argv[i],"--help") )
        {
            Usage(nullptr);
        }
    }

#ifdef MEFU
    MeFu.Print(windowPrincipal," 2 ");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif

/* -------------------------------------------------------------------- */
/*      Set optimal setting for best performance with huge input VRT.   */
/*      The rationale for 450 is that typical Linux process allow       */
/*      only 1024 file descriptors per process and we need to keep some */
/*      spare for shared libraries, etc. so let's go down to 900.       */
/*      And some datasets may need 2 file descriptors, so divide by 2   */
/*      for security.                                                   */
/* -------------------------------------------------------------------- */
    if( CPLGetConfigOption("GDAL_MAX_DATASET_POOL_SIZE", nullptr) == nullptr )
    {
#if defined(__MACH__) && defined(__APPLE__)
        // On Mach, the default limit is 256 files per process
        // TODO We should eventually dynamically query the limit for all OS
        CPLSetConfigOption("GDAL_MAX_DATASET_POOL_SIZE", "100");
#else
        CPLSetConfigOption("GDAL_MAX_DATASET_POOL_SIZE", "450");
#endif
    }

    GDALWarpAppOptionsForBinary* psOptionsForBinary = GDALWarpAppOptionsForBinaryNew();
    /* coverity[tainted_data] */

    GDALWarpAppOptions *psOptions = GDALWarpAppOptionsNew(static_cast<char **>(argv + 1), psOptionsForBinary);

    if( psOptions == nullptr )
    {
        Usage(nullptr);
    }

#ifdef MEFU
    MeFu.Print(windowPrincipal," 3 ");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif

    if( psOptionsForBinary->pszDstFilename == nullptr )
    {
        Usage("No target filename specified.");
    }

    if ( CSLCount(psOptionsForBinary->papszSrcFiles) == 1 &&
         strcmp(psOptionsForBinary->papszSrcFiles[0], psOptionsForBinary->pszDstFilename) == 0 &&
         psOptionsForBinary->bOverwrite)
    {
        CPLError(CE_Failure, CPLE_IllegalArg, "Source and destination datasets must be different.\n");
        GDALExit(1);
    }

/* -------------------------------------------------------------------- */
/*      Open Source files.                                              */
/* -------------------------------------------------------------------- */
    for(int i = 0; psOptionsForBinary->papszSrcFiles[i] != nullptr; i++)
    {
        nSrcCount++;
        pahSrcDS = static_cast<GDALDatasetH *>(CPLRealloc(pahSrcDS, sizeof(GDALDatasetH) * nSrcCount));
        pahSrcDS[nSrcCount-1] = GDALOpenEx( psOptionsForBinary->papszSrcFiles[i], GDAL_OF_RASTER | GDAL_OF_VERBOSE_ERROR, nullptr,
                                            psOptionsForBinary->papszOpenOptions, nullptr );

        if( pahSrcDS[nSrcCount-1] == nullptr )
            GDALExit(2);
    }

#ifdef MEFU
    MeFu.Print(windowPrincipal," 4 ");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif

/* -------------------------------------------------------------------- */
/*      Does the output dataset already exist?                          */
/* -------------------------------------------------------------------- */

    /* FIXME ? source filename=target filename and -overwrite is definitely */
    /* an error. But I can't imagine of a valid case (without -overwrite), */
    /* where it would make sense. In doubt, let's keep that dubious possibility... */

    int bOutStreaming = FALSE;
    if( strcmp(psOptionsForBinary->pszDstFilename, "/vsistdout/") == 0 )
    {
        psOptionsForBinary->bQuiet = TRUE;
        bOutStreaming = TRUE;
    }
#ifdef S_ISFIFO
    else
    {
        VSIStatBufL sStat;
        if( VSIStatExL(psOptionsForBinary->pszDstFilename, &sStat, VSI_STAT_EXISTS_FLAG | VSI_STAT_NATURE_FLAG) == 0 &&
            S_ISFIFO(sStat.st_mode) )
        {
            bOutStreaming = TRUE;
        }
    }
#endif

#ifdef MEFU
    MeFu.Print(windowPrincipal," 5 ");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif

    GDALDatasetH hDstDS = nullptr;
    if( bOutStreaming )
    {
        GDALWarpAppOptionsSetWarpOption(psOptions, "STREAMABLE_OUTPUT", "YES");
    }
    else
    {
        std::vector<CPLErrorHandlerAccumulatorStruct> aoErrors;
        CPLInstallErrorHandlerAccumulator(aoErrors);


        // Aqui está dando o chash /////////////////////
        hDstDS = GDALOpenEx( psOptionsForBinary->pszDstFilename,
                             GDAL_OF_RASTER | GDAL_OF_VERBOSE_ERROR | GDAL_OF_UPDATE ,
                             nullptr, psOptionsForBinary->papszDestOpenOptions, nullptr );
        CPLUninstallErrorHandlerAccumulator();

#ifdef MEFU
        MeFu.Print(windowPrincipal," 6 ");
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif

        if( hDstDS != nullptr )
        {
            for( size_t i = 0; i < aoErrors.size(); i++ )
            {
                CPLError( aoErrors[i].type, aoErrors[i].no, "%s",
                          aoErrors[i].msg.c_str());
            }
        }
    }


#ifdef MEFU
    MeFu.Print(windowPrincipal," 7 ");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif

    if( hDstDS != nullptr && psOptionsForBinary->bOverwrite )
    {
        GDALClose(hDstDS);
        hDstDS = nullptr;
    }

    bool bCheckExistingDstFile =
        !bOutStreaming && hDstDS == nullptr && !psOptionsForBinary->bOverwrite ;

    if( hDstDS != nullptr && psOptionsForBinary->bCreateOutput )
    {
        if( CPLFetchBool(psOptionsForBinary->papszCreateOptions, "APPEND_SUBDATASET", false) )
        {
            GDALClose(hDstDS);
            hDstDS = nullptr;
            bCheckExistingDstFile = false;
        }
        else
        {
            CPLError( CE_Failure, CPLE_AppDefined,
                    "Output dataset %s exists,\n"
                    "but some command line options were provided indicating a new dataset\n"
                    "should be created.  Please delete existing dataset and run again.\n",
                    psOptionsForBinary->pszDstFilename );
            GDALExit(1);
        }
    }

    /* Avoid overwriting an existing destination file that cannot be opened in */
    /* update mode with a new GTiff file */
    if ( bCheckExistingDstFile )
    {
        CPLPushErrorHandler( CPLQuietErrorHandler );
        hDstDS = GDALOpen( psOptionsForBinary->pszDstFilename, GA_ReadOnly );
        CPLPopErrorHandler();

        if (hDstDS)
        {
            CPLError( CE_Failure, CPLE_AppDefined,
                     "Output dataset %s exists, but cannot be opened in update mode\n",
                     psOptionsForBinary->pszDstFilename );
            GDALClose(hDstDS);
            GDALExit(1);
        }
    }

    if( !(psOptionsForBinary->bQuiet) )
    {
        gnSrcCount = nSrcCount;
        GDALWarpAppOptionsSetProgress(psOptions, WarpTermProgress, nullptr);
        GDALWarpAppOptionsSetQuiet(psOptions, false);
    }

    int bUsageError = FALSE;

#ifdef MEFU
    MeFu.Print(windowPrincipal," Aqui chama o Warp.\n");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif
//    return 0;

    GDALDatasetH hOutDS = GDALWarp(psOptionsForBinary->pszDstFilename, hDstDS,
                      nSrcCount, pahSrcDS, psOptions, &bUsageError);

#ifdef MEFU
    MeFu.Print(windowPrincipal," Termino Warp.\n");
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif

    if( bUsageError )
        Usage();
    int nRetCode = (hOutDS) ? 0 : 1;

    GDALWarpAppOptionsFree(psOptions);
    GDALWarpAppOptionsForBinaryFree(psOptionsForBinary);

    // Close first hOutDS since it might reference sources (case of VRT)
    GDALClose( hOutDS ? hOutDS : hDstDS );
    for(int i = 0; i < nSrcCount; i++)
    {
        GDALClose(pahSrcDS[i]);
    }
    CPLFree(pahSrcDS);

    GDALDumpOpenDatasets( stderr );

    GDALDestroyDriverManager();

    OGRCleanupAll();

    return nRetCode;
}
//MAIN_END

/*
int main(int argc,char ** argv) {

    int Num_Elementos = 13;
    int Tamanho = 200;

    char **argv3;
    argv3 = static_cast<char **>(malloc(Tamanho * sizeof(char *)));

    //    char *argv3[200];


    int i = 0;
    for (i = 0 ; i < Num_Elementos ; i ++) {
        argv3[i] = static_cast<char *> (malloc( 80 * sizeof(char)));
    }
    for ( i = Num_Elementos ; i < Tamanho ; i++) {
        argv3[i] = nullptr;
    }
    strcpy(argv3[0],"gdalwarp.exe");
    strcpy(argv3[1],"-of");
    strcpy(argv3[2],"GTiff");
    strcpy(argv3[3],"-srcnodata");
    strcpy(argv3[4],"0");
    strcpy(argv3[5],"-dstnodata");
    strcpy(argv3[6],"0");
    strcpy(argv3[7],"-overwrite");
    strcpy(argv3[8],"-co");
    strcpy(argv3[9],"COMPRESS=JPEG");
    strcpy(argv3[10],"D:/QT_Projects/TST_Warp1/120.tif");
    strcpy(argv3[11],"D:/QT_Projects/TST_Warp1/177.tif");
    strcpy(argv3[12],"D:/QT_Projects/TST_Warp1/Img_Comp_Geo.tif");
//    argv3[11] = nullptr;


    warp_H(Num_Elementos,static_cast<char **>(argv3));

    for (int i = (Num_Elementos -1) ; i >= 0 ; i --) {
        free(argv3[i]);
    }
}
*/
