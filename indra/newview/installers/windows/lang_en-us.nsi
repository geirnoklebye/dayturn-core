; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"

; Language selection dialog
LangString InstallerLanguageTitle  ${LANG_ENGLISH} "Installer Language"
LangString SelectInstallerLanguage  ${LANG_ENGLISH} "Please select the language of the installer"

; subtitle on license text caption
LangString LicenseSubTitleUpdate ${LANG_ENGLISH} " Update"
LangString LicenseSubTitleSetup ${LANG_ENGLISH} " Setup"

LangString MULTIUSER_TEXT_INSTALLMODE_TITLE ${LANG_ENGLISH} "Installation Mode"
LangString MULTIUSER_TEXT_INSTALLMODE_SUBTITLE ${LANG_ENGLISH} "Install for all users (requires Admin) or only for the current user?"
LangString MULTIUSER_INNERTEXT_INSTALLMODE_TOP ${LANG_ENGLISH} "When you run this installer with Admin privileges, you can choose whether to install in (e.g.) c:\Program Files or the current user's AppData\Local folder."
LangString MULTIUSER_INNERTEXT_INSTALLMODE_ALLUSERS ${LANG_ENGLISH} "Install for all users"
LangString MULTIUSER_INNERTEXT_INSTALLMODE_CURRENTUSER ${LANG_ENGLISH} "Install for current user only"

; installation directory text
LangString DirectoryChooseTitle ${LANG_ENGLISH} "Installation Directory" 
LangString DirectoryChooseUpdate ${LANG_ENGLISH} "Select the Dayturn directory to update to version ${VERSION_LONG}.(XXX):"
LangString DirectoryChooseSetup ${LANG_ENGLISH} "Select the directory to install Dayturn in:"

LangString MUI_TEXT_DIRECTORY_TITLE ${LANG_ENGLISH} "Installation Directory"
LangString MUI_TEXT_DIRECTORY_SUBTITLE ${LANG_ENGLISH} "Select the directory into which to install Dayturn:"

LangString MUI_TEXT_INSTALLING_TITLE ${LANG_ENGLISH} "Installing the Dayturn viewer for Opensim..."
LangString MUI_TEXT_INSTALLING_SUBTITLE ${LANG_ENGLISH} "Installing the Dayturn viewer to $INSTDIR"

LangString MUI_TEXT_FINISH_TITLE ${LANG_ENGLISH} "Installing the Dayturn viewer for Opensim"
LangString MUI_TEXT_FINISH_SUBTITLE ${LANG_ENGLISH} "Installed the Dayturn viewer to $INSTDIR."

LangString MUI_TEXT_ABORT_TITLE ${LANG_ENGLISH} "Installation Aborted"
LangString MUI_TEXT_ABORT_SUBTITLE ${LANG_ENGLISH} "Not installing the Dayturn viewer to $INSTDIR."

; CheckStartupParams message box
LangString CheckStartupParamsMB ${LANG_ENGLISH} "Could not find the program '$INSTNAME'. Silent update failed."

; installation success dialog
LangString InstSuccesssQuestion ${LANG_ENGLISH} "Start Dayturn now?"

; remove old NSIS version
LangString RemoveOldNSISVersion ${LANG_ENGLISH} "Checking for old version..."

; check windows version
LangString CheckWindowsVersionDP ${LANG_ENGLISH} "Checking Windows version..."
LangString CheckWindowsVersionMB ${LANG_ENGLISH} "Dayturn only supports Windows Vista with Service Pack 2 and later.$\nInstallation on this Operating System is not supported. Quitting."
LangString CheckWindowsServPackMB ${LANG_ENGLISH} "It is recomended to run Dayturn on the latest service pack for your operating system.$\nThis will help with performance and stability of the program."
LangString UseLatestServPackDP ${LANG_ENGLISH} "Please use Windows Update to install the latest Service Pack."

; checkifadministrator function (install)
LangString CheckAdministratorInstDP ${LANG_ENGLISH} "Checking for permission to install..."
LangString CheckAdministratorInstMB ${LANG_ENGLISH} 'You appear to be using a "limited" account.$\nYou must be an "administrator" to install Dayturn.'

; checkifadministrator function (uninstall)
LangString CheckAdministratorUnInstDP ${LANG_ENGLISH} "Checking for permission to uninstall..."
LangString CheckAdministratorUnInstMB ${LANG_ENGLISH} 'You appear to be using a "limited" account.$\nYou must be an "administrator" to uninstall Dayturn.'

; checkcpuflags
LangString MissingSSE2 ${LANG_ENGLISH} "This machine may not have a CPU with SSE2 support, which is required to run SecondLife ${VERSION_LONG}. Do you want to continue?"

; closesecondlife function (install)
LangString CloseSecondLifeInstDP ${LANG_ENGLISH} "Waiting for Dayturn to shut down..."
LangString CloseSecondLifeInstMB ${LANG_ENGLISH} "Dayturn can't be installed while it is already running.$\n$\nFinish what you're doing then select OK to close Dayturn and continue.$\nSelect CANCEL to cancel installation."
LangString CloseSecondLifeInstRM ${LANG_ENGLISH} "Dayturn failed to remove some files from a previous install."

; closesecondlife function (uninstall)
LangString CloseSecondLifeUnInstDP ${LANG_ENGLISH} "Waiting for Dayturn to shut down..."
LangString CloseSecondLifeUnInstMB ${LANG_ENGLISH} "Dayturn can't be uninstalled while it is already running.$\n$\nFinish what you're doing then select OK to close Dayturn and continue.$\nSelect CANCEL to cancel."

; CheckNetworkConnection
LangString CheckNetworkConnectionDP ${LANG_ENGLISH} "Checking network connection..."

; error during installation
LangString ErrorSecondLifeInstallRetry ${LANG_ENGLISH} "Dayturn installer encountered issues during installation. Some files may not have been copied correctly."
LangString ErrorSecondLifeInstallSupport ${LANG_ENGLISH} "Please reinstall viewer from https://www.dayturn.com/viewer/index.php?resources/ and contact the Support section of https://www.dayturn.com/viewer/ if issue persists after reinstall."

; ask to remove user's data files
LangString RemoveDataFilesMB ${LANG_ENGLISH} "Do you want to REMOVE all other files related to Dayturn as well?$\n$\nIt is recomended to keep the settings and cache files if you have other versions of Dayturn installed or are uninstalling to upgrade to a newer version."

; Install folder still contains files
LangString DeleteProgramFilesMB ${LANG_ENGLISH} "There are still files in your Dayturn program directory.$\n$\nThese are possibly files you created or moved to:$\n$INSTDIR$\n$\nDo you want to remove them?"

; uninstall text
LangString UninstallTextMsg ${LANG_ENGLISH} "This will uninstall Dayturn ${VERSION_LONG} from your system."

; ask to remove registry keys that still might be needed by other viewers that are installed
LangString DeleteRegistryKeysMB ${LANG_ENGLISH} "Do you want to remove application registry keys?$\n$\nIt is recomended to keep registry keys if you have other versions of Dayturn installed."
