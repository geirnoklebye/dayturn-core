; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Polish.nlf"

; Language selection dialog
LangString InstallerLanguageTitle  ${LANG_POLISH} "Język instalatora"
LangString SelectInstallerLanguage  ${LANG_POLISH} "Proszę wybrać język instalatora"

; subtitle on license text caption
LangString LicenseSubTitleUpdate ${LANG_POLISH} " Aktualizacja"
LangString LicenseSubTitleSetup ${LANG_POLISH} " Instalacja"

LangString MULTIUSER_TEXT_INSTALLMODE_TITLE ${LANG_POLISH} "Installation Mode"
LangString MULTIUSER_TEXT_INSTALLMODE_SUBTITLE ${LANG_POLISH} "Install for all users (requires Admin) or only for the current user?"
LangString MULTIUSER_INNERTEXT_INSTALLMODE_TOP ${LANG_POLISH} "When you run this installer with Admin privileges, you can choose whether to install in (e.g.) c:\Program Files or the current user's AppData\Local folder."
LangString MULTIUSER_INNERTEXT_INSTALLMODE_ALLUSERS ${LANG_POLISH} "Install for all users"
LangString MULTIUSER_INNERTEXT_INSTALLMODE_CURRENTUSER ${LANG_POLISH} "Install for current user only"

; installation directory text
LangString DirectoryChooseTitle ${LANG_POLISH} "Katalog instalacji" 
LangString DirectoryChooseUpdate ${LANG_POLISH} "Wybierz katalog instalacji Dayturn w celu aktualizacji wersji ${VERSION_LONG}.(XXX):"
LangString DirectoryChooseSetup ${LANG_POLISH} "Wybierz katalog instalacji Dayturn w:"

LangString MUI_TEXT_DIRECTORY_TITLE ${LANG_POLISH} "Installation Directory"
LangString MUI_TEXT_DIRECTORY_SUBTITLE ${LANG_POLISH} "Select the directory into which to install Dayturn:"

LangString MUI_TEXT_INSTALLING_TITLE ${LANG_POLISH} "Installing Dayturn..."
LangString MUI_TEXT_INSTALLING_SUBTITLE ${LANG_POLISH} "Installing the Dayturn viewer to $INSTDIR"

LangString MUI_TEXT_FINISH_TITLE ${LANG_POLISH} "Installing Dayturn"
LangString MUI_TEXT_FINISH_SUBTITLE ${LANG_POLISH} "Installed the Dayturn viewer to $INSTDIR."

LangString MUI_TEXT_ABORT_TITLE ${LANG_POLISH} "Installation Aborted"
LangString MUI_TEXT_ABORT_SUBTITLE ${LANG_POLISH} "Not installing the Dayturn viewer to $INSTDIR."

; CheckStartupParams message box
LangString CheckStartupParamsMB ${LANG_POLISH} "Nie można odnaleźć programu '$INSTNAME'. Cicha aktualizacja zakończyła się niepowodzeniem."

; installation success dialog
LangString InstSuccesssQuestion ${LANG_POLISH} "Czy uruchomić Dayturn teraz?"

; remove old NSIS version
LangString RemoveOldNSISVersion ${LANG_POLISH} "Poszukiwanie starszej wersji..."

; check windows version
LangString CheckWindowsVersionDP ${LANG_POLISH} "Sprawdzanie wersji Windows..."
LangString CheckWindowsVersionMB ${LANG_POLISH} 'Dayturn obsługuje tylko Windows Vista.$\n$\nPróba zainstalowania na Windows $R0 może spowodować awarie i utratę danych.$\n$\n'
LangString CheckWindowsServPackMB ${LANG_POLISH} "Zalecane jest uruchamianie Dayturn z najnowszym dostępnym Service Packiem zainstalowanym w systemie.$\nPomaga on w podniesieniu wydajności i stabilności programu."
LangString UseLatestServPackDP ${LANG_POLISH} "Użyj usługi Windows Update, aby zainstalować najnowszy Service Pack."

; checkifadministrator function (install)
LangString CheckAdministratorInstDP ${LANG_POLISH} "Sprawdzanie zezwolenia na instalację..."
LangString CheckAdministratorInstMB ${LANG_POLISH} 'Używasz "ograniczonego" konta.$\nMusisz być zalogowany jako "administrator" aby zainstalować Dayturn.'

; checkifadministrator function (uninstall)
LangString CheckAdministratorUnInstDP ${LANG_POLISH} "Sprawdzanie zezwolenia na odinstalowanie..."
LangString CheckAdministratorUnInstMB ${LANG_POLISH} 'Używasz "ograniczonego" konta.$\nMusisz być być zalogowany jako "administrator" aby zainstalować Dayturn.'

; checkifalreadycurrent
LangString CheckIfCurrentMB ${LANG_POLISH} "Dayturn ${VERSION_LONG} jest już zainstalowane.$\n$\nCzy chcesz zainstalować Dayturn ponownie?"

; checkcpuflags
LangString MissingSSE2 ${LANG_POLISH} "Ten komputer może nie mieć procesora z obsługą SSE2, który jest wymagany aby uruchomić Dayturn w wersji ${VERSION_LONG}. Chcesz kontynuować?"

; closesecondlife function (install)
LangString CloseSecondLifeInstDP ${LANG_POLISH} "Oczekiwanie na zamknięcie Dayturn..."
LangString CloseSecondLifeInstMB ${LANG_POLISH} "Dayturn nie może zostać zainstalowane, ponieważ jest już włączone.$\n$\nZakończ swoje działania i wybierz OK aby zamknąć Dayturn i kontynuować.$\nWybierz CANCEL aby anulować instalację."
LangString CloseSecondLifeInstRM ${LANG_POLISH} "Dayturn failed to remove some files from a previous install."

; closesecondlife function (uninstall)
LangString CloseSecondLifeUnInstDP ${LANG_POLISH} "Oczekiwanie na zamknięcie Dayturn..."
LangString CloseSecondLifeUnInstMB ${LANG_POLISH} "Dayturn nie może zostać zainstalowane, ponieważ jest już włączone.$\n$\nZakończ swoje działania i wybierz OK aby zamknąć Dayturn i kontynuować.$\nWybierz CANCEL aby anulować."

; CheckNetworkConnection
LangString CheckNetworkConnectionDP ${LANG_POLISH} "Sprawdzanie połączenia sieciowego..."

; delete program files
LangString DeleteProgramFilesMB ${LANG_POLISH} "Nadal istnieją pliki w katalogu instalacyjnym Dayturn.$\n$\nMożliwe, że są to pliki, które stworzyłeś/stworzyłaś lub przeniosłeś/przeniosłaś do:$\n$INSTDIR$\n$\nCzy chcesz je usunąć?"

; uninstall text
LangString UninstallTextMsg ${LANG_POLISH} "To spowoduje odinstalowanie Dayturn ${VERSION_LONG} z Twojego systemu."
