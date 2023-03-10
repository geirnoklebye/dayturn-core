; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Danish.nlf"

; Language selection dialog
LangString InstallerLanguageTitle  ${LANG_DANISH} "Installationssprog"
LangString SelectInstallerLanguage  ${LANG_DANISH} "Vælg venligst sprog til installation"

; subtitle on license text caption
LangString LicenseSubTitleUpdate ${LANG_DANISH} " Opdater"
LangString LicenseSubTitleSetup ${LANG_DANISH} " Opsætning"

LangString MULTIUSER_TEXT_INSTALLMODE_TITLE ${LANG_DANISH} "Installation Mode"
LangString MULTIUSER_TEXT_INSTALLMODE_SUBTITLE ${LANG_DANISH} "Install for all users (requires Admin) or only for the current user?"
LangString MULTIUSER_INNERTEXT_INSTALLMODE_TOP ${LANG_DANISH} "When you run this installer with Admin privileges, you can choose whether to install in (e.g.) c:\Program Files or the current user's AppData\Local folder."
LangString MULTIUSER_INNERTEXT_INSTALLMODE_ALLUSERS ${LANG_DANISH} "Install for all users"
LangString MULTIUSER_INNERTEXT_INSTALLMODE_CURRENTUSER ${LANG_DANISH} "Install for current user only"

; installation directory text
LangString DirectoryChooseTitle ${LANG_DANISH} "Installationsmappe" 
LangString DirectoryChooseUpdate ${LANG_DANISH} "Vælg Dayturn mappe til opdatering til version ${VERSION_LONG}.(XXX):"
LangString DirectoryChooseSetup ${LANG_DANISH} "Vælg mappe hvor Dayturn skal installeres:"

LangString MUI_TEXT_DIRECTORY_TITLE ${LANG_DANISH} "Installation Directory"
LangString MUI_TEXT_DIRECTORY_SUBTITLE ${LANG_DANISH} "Select the directory into which to install Dayturn:"

LangString MUI_TEXT_INSTALLING_TITLE ${LANG_DANISH} "Installing Dayturn..."
LangString MUI_TEXT_INSTALLING_SUBTITLE ${LANG_DANISH} "Installing the Dayturn viewer to $INSTDIR"

LangString MUI_TEXT_FINISH_TITLE ${LANG_DANISH} "Installing Dayturn"
LangString MUI_TEXT_FINISH_SUBTITLE ${LANG_DANISH} "Installed the Dayturn viewer to $INSTDIR."

LangString MUI_TEXT_ABORT_TITLE ${LANG_DANISH} "Installation Aborted"
LangString MUI_TEXT_ABORT_SUBTITLE ${LANG_DANISH} "Not installing the Dayturn viewer to $INSTDIR."

; CheckStartupParams message box
LangString CheckStartupParamsMB ${LANG_DANISH} "Kunne ikke finde programmet '$INSTNAME'. Baggrundsopdatering fejlede."

; installation success dialog
LangString InstSuccesssQuestion ${LANG_DANISH} "Start Dayturn nu?"

; remove old NSIS version
LangString RemoveOldNSISVersion ${LANG_DANISH} "Checker ældre version..."

; check windows version
LangString CheckWindowsVersionDP ${LANG_DANISH} "Checker Windows version..."
LangString CheckWindowsVersionMB ${LANG_DANISH} 'Dayturn supporterer kun Windows Vista.$\n$\nForsøg på installation på Windows $R0 kan resultere i nedbrud og datatab.$\n$\n'
LangString CheckWindowsServPackMB ${LANG_DANISH} "It is recomended to run Dayturn on the latest service pack for your operating system.$\nThis will help with performance and stability of the program."
LangString UseLatestServPackDP ${LANG_DANISH} "Please use Windows Update to install the latest Service Pack."

; checkifadministrator function (install)
LangString CheckAdministratorInstDP ${LANG_DANISH} "Checker rettigheder til installation..."
LangString CheckAdministratorInstMB ${LANG_DANISH} 'Det ser ud til at du benytter en konto med begrænsninger.$\nDu skal have "administrator" rettigheder for at installere Dayturn.'

; checkifadministrator function (uninstall)
LangString CheckAdministratorUnInstDP ${LANG_DANISH} "Checker rettigheder til at afinstallere..."
LangString CheckAdministratorUnInstMB ${LANG_DANISH} 'Det ser ud til at du benytter en konto med begrænsninger.$\nDu skal have "administrator" rettigheder for at afinstallere Dayturn.'

; checkifalreadycurrent
LangString CheckIfCurrentMB ${LANG_DANISH} "Det ser ud til at Dayturn ${VERSION_LONG} allerede er installeret.$\n$\nØnsker du at installere igen?"

; checkcpuflags
LangString MissingSSE2 ${LANG_DANISH} "This machine may not have a CPU with SSE2 support, which is required to run SecondLife ${VERSION_LONG}. Do you want to continue?"

; closesecondlife function (install)
LangString CloseSecondLifeInstDP ${LANG_DANISH} "Venter på at Dayturn skal lukke ned..."
LangString CloseSecondLifeInstMB ${LANG_DANISH} "Dayturn kan ikke installeres mens programmet kører.$\n$\nAfslut programmet for at fortsætte.$\nVælg ANNULÉR for at afbryde installation."
LangString CloseSecondLifeInstRM ${LANG_DANISH} "Dayturn failed to remove some files from a previous install."

; closesecondlife function (uninstall)
LangString CloseSecondLifeUnInstDP ${LANG_DANISH} "Venter på at Dayturn skal lukke ned..."
LangString CloseSecondLifeUnInstMB ${LANG_DANISH} "Dayturn kan ikke afinstalleres mens programmet kører.$\n$\nAfslut programmet for at fortsætte.$\nVælg ANNULÉR for at afbryde installation."

; CheckNetworkConnection
LangString CheckNetworkConnectionDP ${LANG_DANISH} "Checker netværksforbindelse..."

; delete program files
LangString DeleteProgramFilesMB ${LANG_DANISH} "Der er stadig filer i Dayturn program mappen.$\n$\nDette er sandsynligvis filer du har oprettet eller flyttet til :$\n$INSTDIR$\n$\nØnsker du at fjerne disse filer?"

; uninstall text
LangString UninstallTextMsg ${LANG_DANISH} "Dette vil afinstallere Dayturn ${VERSION_LONG} fra dit system."
