import defines
import os
import subprocess


def dmpLogName(dmpName):
  if (len(dmpName) <= 0):
    return "";
  return os.path.splitext(dmpName)[0] + ".log";

def getDmpVerbose(dmpPath):
  if not os.path.isfile(dmpPath):
    return;
  dmpName = os.path.basename(dmpPath);
  if len(dmpName) < 0:
    return;
  logName = dmpLogName(dmpName);
  logPath = os.path.join(defines.DMPS_LOGS_DIR, logName);

  cdb = '"' + defines.CDB_EXE + '"';
  yParam = '-y ' + '"' + defines.SYMBOL_PATHS + '"';
  zParam = '-z ' + '"' +dmpPath + '"';
  oParam = '-v -c ".ecxr;  ;kb L1000;  ; q"';
  rParam = '> ' + '"' + logPath + '"'
  
  #cmd = cdb + " " + yParam + " " + zParam + " " + oParam + " " + rParam;
  #print(cmd + "\n");
  #ps = subprocess.call(cmd, shell = True); # 执行cmd命令
  
  rParam = '';
  cmd = cdb + " " + yParam + " " + zParam + " " + oParam + " " + rParam;
  print(cmd + "\n");
  pip = subprocess.Popen(cmd, stdout=subprocess.PIPE);
  out = pip.communicate();
  
  return out[0];
  
def extractCallstack(verbose):
  dataLen = len(verbose);
  if (dataLen <= 0):
    return "";
  lpos = 0;
  rpos = 0;
  le = "";
  ret = "";
  findBegin = False;
  while (True):
    rpos = verbose.find(b'\n', lpos);
    le = verbose[lpos: rpos];
    if not findBegin:
      if (le.startswith(b"ChildEBP")):
        findBegin = True;
        ret += str(le);
        ret += "\n";
    else:
      if (le.startswith(b"quit")):
        ret += str(le);
        ret += "\n";
        break;
      ret += str(le);
      ret += "\n";
    if (rpos == -1):
      break;
    lpos = rpos + 1;
    if (lpos >= dataLen):
      break;
  return ret;
  
def writeCallstack(section, cs, f):
  f.write(section);
  f.write("\n");
  f.write(cs);
  f.write("\n");
  f.write("\n");
  f.write("\n");
  
def openCsLog():
  if not os.path.exists(defines.DMPS_LOGS_DIR):
    os.makedirs(defines.DMPS_LOGS_DIR);
  return open(defines.DMPS_LOGS_DIR + "\callstack.log", "w");
  

def analyzeDmps():
  list = os.listdir(defines.DMPS_DIRS);
  f = openCsLog();
  for i in range(0, len(list)):
    path = os.path.join(defines.DMPS_DIRS, list[i]);
    if os.path.isfile(path):
      vs = getDmpVerbose(path);
      cs = extractCallstack(vs);
      writeCallstack(path, cs, f);
    f.close();

if __name__ == "__main__":
  analyzeDmps()