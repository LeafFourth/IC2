set RESOURCES_DIR=%~dp0..\Resources
set EXE_DIR=%1

xcopy /E /Y /F %RESOURCES_DIR% %EXE_DIR%\resources\
