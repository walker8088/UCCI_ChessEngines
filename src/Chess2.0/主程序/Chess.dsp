# Microsoft Developer Studio Project File - Name="Chess" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Chess - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Chess.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Chess.mak" CFG="Chess - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Chess - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Chess - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Chess - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "Chess - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Chess - Win32 Release"
# Name "Chess - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BaseEveluation.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseSearchEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Chess.cpp
# End Source File
# Begin Source File

SOURCE=.\Chess.rc
# End Source File
# Begin Source File

SOURCE=.\ChessDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CoolButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Eveluation.cpp
# End Source File
# Begin Source File

SOURCE=.\GradientProgressCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HistoryHeuristic.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\MemDC.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH.cpp
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH2.cpp
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH3.cpp
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH4.cpp
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH5.cpp
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH6.cpp
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH7.cpp
# End Source File
# Begin Source File

SOURCE=.\PublicFun.cpp
# End Source File
# Begin Source File

SOURCE=.\SetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TranspositionTable.cpp
# End Source File
# Begin Source File

SOURCE=.\XPButton.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BaseEveluation.h
# End Source File
# Begin Source File

SOURCE=.\BaseSearchEngine.h
# End Source File
# Begin Source File

SOURCE=.\Chess.h
# End Source File
# Begin Source File

SOURCE=.\ChessDlg.h
# End Source File
# Begin Source File

SOURCE=.\CoolButton.h
# End Source File
# Begin Source File

SOURCE=.\Define.h
# End Source File
# Begin Source File

SOURCE=.\Eveluation.h
# End Source File
# Begin Source File

SOURCE=.\GradientProgressCtrl.h
# End Source File
# Begin Source File

SOURCE=.\HelpDlg.h
# End Source File
# Begin Source File

SOURCE=.\HistoryHeuristic.h
# End Source File
# Begin Source File

SOURCE=.\HyperLink.h
# End Source File
# Begin Source File

SOURCE=.\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\MoveGenerator.h
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH.h
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH2.h
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH3.h
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH4.h
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH5.h
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH6.h
# End Source File
# Begin Source File

SOURCE=.\NegaScout_TT_HH7.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SetDlg.h
# End Source File
# Begin Source File

SOURCE=.\SplashDlg.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TipDlg.h
# End Source File
# Begin Source File

SOURCE=.\TranspositionTable.h
# End Source File
# Begin Source File

SOURCE=.\XPButton.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BoxFrom.bmp
# End Source File
# Begin Source File

SOURCE=.\res\BoxTo.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cancle.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Chess.ico
# End Source File
# Begin Source File

SOURCE=.\res\Chess.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ChessBoard.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ChessMan.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cut.ico
# End Source File
# Begin Source File

SOURCE=".\res\Help GID.ico"
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon3.ico
# End Source File
# Begin Source File

SOURCE=.\res\litebulb.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ok.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Splash.bmp
# End Source File
# Begin Source File

SOURCE=".\res\ÐÂ½¨Í¼±ê(2).ico"
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section Chess : {72ADFD54-2C39-11D0-9903-00A0C91BC942}
# 	1:17:CG_IDS_DIDYOUKNOW:109
# 	1:22:CG_IDS_TIPOFTHEDAYMENU:108
# 	1:18:CG_IDS_TIPOFTHEDAY:107
# 	1:22:CG_IDS_TIPOFTHEDAYHELP:112
# 	1:19:CG_IDP_FILE_CORRUPT:111
# 	1:7:IDD_TIP:106
# 	1:13:IDB_LIGHTBULB:105
# 	1:18:CG_IDS_FILE_ABSENT:110
# 	2:17:CG_IDS_DIDYOUKNOW:CG_IDS_DIDYOUKNOW
# 	2:7:CTipDlg:CTipDlg
# 	2:22:CG_IDS_TIPOFTHEDAYMENU:CG_IDS_TIPOFTHEDAYMENU
# 	2:18:CG_IDS_TIPOFTHEDAY:CG_IDS_TIPOFTHEDAY
# 	2:12:CTIP_Written:OK
# 	2:22:CG_IDS_TIPOFTHEDAYHELP:CG_IDS_TIPOFTHEDAYHELP
# 	2:2:BH:
# 	2:19:CG_IDP_FILE_CORRUPT:CG_IDP_FILE_CORRUPT
# 	2:7:IDD_TIP:IDD_TIP
# 	2:8:TipDlg.h:TipDlg.h
# 	2:13:IDB_LIGHTBULB:IDB_LIGHTBULB
# 	2:18:CG_IDS_FILE_ABSENT:CG_IDS_FILE_ABSENT
# 	2:10:TipDlg.cpp:TipDlg.cpp
# End Section
