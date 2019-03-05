set ROOT_DIR=%~dp0..\

xcopy  /Y %ROOT_DIR%deps\dll %ROOT_DIR%build\Debug\bin\
xcopy  /Y %ROOT_DIR%deps\dll %ROOT_DIR%build\Release\bin\