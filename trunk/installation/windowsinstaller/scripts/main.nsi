# Script for installing SpikeStream 0.2 on Windows

#------------------------------------------------#
#-----             Basic Information        -----#
#------------------------------------------------#

# Root directory of SpikeStream
!define SPIKESTREAM_ROOT ..\..\..

# Location of other files
!define QT_ROOT D:\Qt\4.7.2
!define MINGW_ROOT D:\MinGW
!define MYSQL_ROOT D:\MySQL
#!define QT_ROOT C:\Qt\4.7.3
#!define MINGW_ROOT C:\MinGW
#!define MYSQL_LIB C:\MySQL\lib\opt


# The name of the installer
Name "SpikeStream 0.2"

# The file to write
outFile "../build/spikestream-windows-setup-0.2.exe"

# The default installation directory 
InstallDir $PROGRAMFILES\spikestream-0.2


#------------------------------------------------#
#-----              Pages                   -----#
#------------------------------------------------#
Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles


#------------------------------------------------#
#-----              License                 -----#
#------------------------------------------------#
LicenseData "gpl-2.0.txt"


#------------------------------------------------#
#-----              Shortcuts               -----#
#------------------------------------------------#
# Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\SpikeStream"
  CreateShortCut "$SMPROGRAMS\SpikeStream\Uninstall SpikeStream.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\SpikeStream\SpikeStream.lnk" "$INSTDIR\bin\spikestream.exe" "" "$INSTDIR\bin\spikestream.exe" 0
  CreateShortCut "$SMPROGRAMS\SpikeStream\Database Configuration.lnk" "$INSTDIR\bin\dbconfigtool.exe" "" "$INSTDIR\bin\dbconfigtool.exe" 0
SectionEnd

Section "Desktop Shortcut"
  CreateShortCut "$DESKTOP\SpikeStream.lnk" "$INSTDIR\bin\spikestream.exe" "" "$INSTDIR\bin\spikestream.exe" 0
SectionEnd

Section "Quick Launch Shortcut"
  CreateShortCut "$QUICKLAUNCH\SpikeStream.lnk" "$INSTDIR\bin\spikestream.exe" "" "$INSTDIR\bin\spikestream.exe" 0
SectionEnd


#------------------------------------------------#
#-----              Files                   -----#
#------------------------------------------------#
Section ""

  # Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  # List of files to install
  !include files.nsh
  
  # Output the uninstaller
  WriteUninstaller "uninstall.exe"
  
SectionEnd # end the section


#------------------------------------------------#
#-----              Uninstall               -----#
#------------------------------------------------#
Section "Uninstall"

#  MessageBox MB_OK "$SMPROGRAMS"
	
  # Remove files at the root of the installation directory
  Delete $INSTDIR\spikestream.config.template
  Delete $INSTDIR\spikestream.config
  Delete $INSTDIR\uninstall.exe
  
  # Remove shortcuts
  Delete "$DESKTOP\SpikeStream.lnk"
  Delete "$QUICKLAUNCH\SpikeStream.lnk"
  Delete "$SMPROGRAMS\SpikeStream\SpikeStream.lnk"
  Delete "$SMPROGRAMS\SpikeStream\Uninstall SpikeStream.lnk"
  RMDir "$SMPROGRAMS\SpikeStream"

  # Remove directories 
  RMDir /r $INSTDIR\bin\sqldrivers
  RMDir /r $INSTDIR\bin
  RMDir /r $INSTDIR\database
  RMDir /r $INSTDIR\doc
  RMDir /r $INSTDIR\log
  RMDir /r $INSTDIR\images
  RMDir /r $INSTDIR\plugins
  RMDir $INSTDIR

SectionEnd


