# Microsoft Developer Studio Project File - Name="wstock" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=wstock - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wstock.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wstock.mak" CFG="wstock - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wstock - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /GX /O2 /D "WIN32" /D "NDEBUG" /D "__WXMSW__" /D "_MBCS" /D "_WINDOWS" /D "NOPCH" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wxmsw26_xrc.lib wxmsw26_html.lib wxmsw26_adv.lib wxmsw26_core.lib wxbase26_xml.lib wxbase26.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib libcurl_imp.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib comctl32.lib rpcrt4.lib wsock32.lib libxml2.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"library"
# SUBTRACT LINK32 /pdb:none
# Begin Target

# Name "wstock - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\app.cpp
# End Source File
# Begin Source File

SOURCE=.\gspreadsheet.cpp
# End Source File
# Begin Source File

SOURCE=.\htmltableparser.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\MyDayInfoDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\MyDayInfoDialog.h
# End Source File
# Begin Source File

SOURCE=.\mystockdetail.cpp
# End Source File
# Begin Source File

SOURCE=.\mystockdetail.h
# End Source File
# Begin Source File

SOURCE=.\MyStockDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\sinastock.cpp
# End Source File
# Begin Source File

SOURCE=.\sohustock.cpp
# End Source File
# Begin Source File

SOURCE=.\sohustock.h
# End Source File
# Begin Source File

SOURCE=.\StockHistoryDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\stockplot.cpp
# End Source File
# Begin Source File

SOURCE=.\stocks.cpp
# End Source File
# Begin Source File

SOURCE=.\wstockcustomdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\wstockgeturl.cpp
# End Source File
# Begin Source File

SOURCE=.\wstockglobalinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\wstockglobalinfo.h
# End Source File
# Begin Source File

SOURCE=.\yahoostock.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\app.h
# End Source File
# Begin Source File

SOURCE=.\gspreadsheet.h
# End Source File
# Begin Source File

SOURCE=.\htmltableparser.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\MyStockDialog.h
# End Source File
# Begin Source File

SOURCE=.\sinastock.h
# End Source File
# Begin Source File

SOURCE=.\StockHistoryDialog.h
# End Source File
# Begin Source File

SOURCE=.\stockplot.h
# End Source File
# Begin Source File

SOURCE=.\stocks.h
# End Source File
# Begin Source File

SOURCE=.\wstockconfig.h
# End Source File
# Begin Source File

SOURCE=.\wstockconst.h
# End Source File
# Begin Source File

SOURCE=.\wstockcustomdialog.h
# End Source File
# Begin Source File

SOURCE=.\wstockgeturl.h
# End Source File
# Begin Source File

SOURCE=.\wx_pch.h
# End Source File
# Begin Source File

SOURCE=.\yahoostock.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\wstock.ico
# End Source File
# End Group
# End Target
# End Project
