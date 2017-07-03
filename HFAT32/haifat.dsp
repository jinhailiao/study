# Microsoft Developer Studio Project File - Name="haifat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=haifat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "haifat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "haifat.mak" CFG="haifat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "haifat - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "haifat - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "haifat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".\Haifat" /I ".\Haifat\fat" /I ".\Haifat\device" /I ".\Haifat\driver" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "haifat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\Haifat" /I ".\Haifat\fat" /I ".\Haifat\device" /I ".\Haifat\driver" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "haifat - Win32 Release"
# Name "haifat - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "device"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Haifat\device\ftdevice.c
# End Source File
# End Group
# Begin Group "driver"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Haifat\driver\nand2410.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\driver\ramdisk.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\driver\ramdisk2.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\driver\ramdisk3.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\driver\sdi2410.c
# End Source File
# End Group
# Begin Group "fat"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Haifat\fat\hfatdent.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatfile.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatfind.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatstr.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatsys.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatutil.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\Haifat\fssignal.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\haiclib.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\hfileapi.c
# End Source File
# Begin Source File

SOURCE=.\Haifat\hfilesys.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Haifat\fsconfig.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\fssignal.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\device\ftdevice.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\haiclib.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatdent.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatfile.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatfind.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatstr.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatsys.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfatutil.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\hfileapi.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\fat\hfilesys.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\hfserr.h
# End Source File
# Begin Source File

SOURCE=.\Haifat\hfstype.h
# End Source File
# End Group
# End Target
# End Project
