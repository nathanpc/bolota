# Microsoft Developer Studio Project File - Name="Bolota" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Bolota - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Bolota.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Bolota.mak" CFG="Bolota - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Bolota - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Bolota - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Bolota - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\shims\win32\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib shlwapi.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Bolota - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\shims\win32\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib shlwapi.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Bolota - Win32 Release"
# Name "Bolota - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE="..\icons\bolota-icon.ico"
# End Source File
# Begin Source File

SOURCE=.\Bolota.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=..\src\SharedResources.h
# End Source File
# End Group
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Application.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Application.h
# End Source File
# Begin Source File

SOURCE=..\src\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=..\src\stdafx.h
# End Source File
# Begin Source File

SOURCE=..\src\targetver.h
# End Source File
# End Group
# Begin Group "Bolota"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Bolota\DateField.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Bolota\DateField.h
# End Source File
# Begin Source File

SOURCE=..\src\Bolota\Document.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Bolota\Document.h
# End Source File
# Begin Source File

SOURCE=..\src\Bolota\Exceptions.h
# End Source File
# Begin Source File

SOURCE=..\src\Bolota\Field.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Bolota\Field.h
# End Source File
# Begin Source File

SOURCE=..\src\Bolota\UString.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Bolota\UString.h
# End Source File
# End Group
# Begin Group "Windows and Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\AboutDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\AboutDialog.h
# End Source File
# Begin Source File

SOURCE=..\src\FieldManagerDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\FieldManagerDialog.h
# End Source File
# Begin Source File

SOURCE=..\src\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\MainWindow.h
# End Source File
# Begin Source File

SOURCE=..\src\PropertiesDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\src\PropertiesDialog.h
# End Source File
# End Group
# Begin Group "Components"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Components\BolotaView.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Components\BolotaView.h
# End Source File
# End Group
# Begin Group "Exceptions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Exceptions\SystemException.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Exceptions\SystemException.h
# End Source File
# End Group
# Begin Group "Utilities"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Utilities\DialogWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Utilities\DialogWindow.h
# End Source File
# Begin Source File

SOURCE=..\src\Utilities\MsgBoxes.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Utilities\MsgBoxes.h
# End Source File
# Begin Source File

SOURCE=..\src\Utilities\WindowUtilities.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Utilities\WindowUtilities.h
# End Source File
# End Group
# End Target
# End Project