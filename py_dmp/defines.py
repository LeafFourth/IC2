import os

SYMBOL_PATHS = "srv*d:\\symbolslocal*http://msdl.microsoft.com/download/symbols;C:\\Users\\admin\\Desktop\\dmp;C:\\Users\\admin\\Desktop\\dmp\\1.3.1.2;C:\\Users\\admin\\Desktop\\dmp\\1.3.1.1_0";
CDB_EXE = r"C:\Program Files (x86)\Debugging Tools for Windows\cdb.exe";
DMPS_DIRS = os.path.split(__file__)[0] + "\\test\\dmps";
DMPS_LOGS_DIR = os.path.split(__file__)[0] + "\\test\\logs";