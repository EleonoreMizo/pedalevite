
@set tmp=tmp

@cd /d "%~dp0"

@rmdir /s /q "%tmp%"

@mkdir "%tmp%"
@mkdir "%tmp%\doc"
@mkdir "%tmp%\build"
@mkdir "%tmp%\build\win"
@mkdir "%tmp%\etc"
@mkdir "%tmp%\etc\config"
@copy "doc\drawing-panel.dxf"                  "%tmp%\doc"
@copy "doc\exported\drawing-panel.pdf"         "%tmp%\doc"
@copy "doc\exported\drawing-panel.png"         "%tmp%\doc"
@copy "doc\drawing-pi-mount.dxf"               "%tmp%\doc"
@copy "doc\exported\drawing-pi-mount.pdf"      "%tmp%\doc"
@copy "doc\exported\drawing-pi-mount.png"      "%tmp%\doc"
@copy "doc\cables.ods"                         "%tmp%\doc"
@copy "doc\parts.ods"                          "%tmp%\doc"
@copy "doc\schematics.fzz"                     "%tmp%\doc"
@copy "doc\exported\schematics-board.pdf"      "%tmp%\doc"
@copy "doc\exported\schematics-board.png"      "%tmp%\doc"
@copy "doc\exported\schematics-schematics.pdf" "%tmp%\doc"
@copy "doc\exported\schematics-schematics.png" "%tmp%\doc"
@copy "etc\config\current"                     "%tmp%\etc\config"
@xcopy /I /E "art"                             "%tmp%\art"
@xcopy /I /E "bin"                             "%tmp%\bin"
@xcopy /I /E "build\unix"                      "%tmp%\build\unix"
@xcopy /I /E "build\win"                       "%tmp%\build\win" /EXCLUDE:build\win\exclude.txt
@xcopy /I /E "src"                             "%tmp%\src"

cd "%tmp%"
@date /T > d.txt
@set /P today= < d.txt
@del d.txt
@set filename=pedalevite-%today%.zip
@if exist "..\%filename%" del "..\%filename%"
@echo Pédale Vite | "C:\Program Files (x86)\Infozip\zip.exe" -r -o -9 -z "..\%filename%" "*.*"
@cd ..

@rmdir /s /q "%tmp%"

@pause
