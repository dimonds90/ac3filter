import os, sys, re, time
from os import chdir
from os.path import exists

if not exists('config.py'):
  raise('Copy config.py.example to config.py and adjust configuration')
from config import *

###########################################################
# Helper funcs

def run(command):
  print '> ' + command

  # Command must be quoted.
  # Otherwise, quotes at the command itself do not work. Strange, but true!
  result = os.system('"' + command + '"')
  if result != 0:
    raise Exception(command.split(' ')[0] + 
      " failed with error code " + str(result))

def run_at(dir, command):
  old_dir = os.getcwd()
  os.chdir(dir)
  run(command)
  chdir(old_dir)

def split_ver(ver):
  m = re.match(r'(\d+)\.(\d+)(.?)', ver)
  if not m:
    raise Exception('Incorrect version format.')
  return m.groups()  

def make_filename_ver(ver_dot):
  return ver_dot.replace('.', '_')

def update_ac3filter_ver(file, ver):
  result = []
  major, minor, status = split_ver(ver)
  for str in open(file):
    if str.find('#define AC3FILTER_VER_MAJOR') == 0:
      result.append('#define AC3FILTER_VER_MAJOR %s\n' % major)
    elif str.find('#define AC3FILTER_VER_MINOR') == 0:
      result.append('#define AC3FILTER_VER_MINOR %s\n' % minor)
    elif str.find('#define AC3FILTER_VER_STATE') == 0:
      result.append('#define AC3FILTER_VER_STATE "%s"\n' % status)
    else:
      result.append(str)
  open(file, 'w').write(''.join(result))

def update_changelog(file, ver):
  result = open(file).readlines()
  ver_string = 'v%s - %s\n' % (ver, time.strftime('%Y-%m-%d', time.localtime()))
  if result[0][0] != ' ':
    result[0] = ver_string
  else:
    result[0:0] = ver_string
  open(file, 'w').write(''.join(result))

def sec2hms(sec):
  return ( sec // 3600, sec % 3600 // 60, sec % 60 )

###########################################################

start_time = time.time()

###########################################################
# Version

ver_test = 'test'

if len(sys.argv) > 1:
  ver = sys.argv[1]
  ver_file = make_filename_ver(ver)

  ans = raw_input('Are you sure you want to commit version {} (Y/n)?'.format(ver));
  if ans != 'Y':
    sys.exit(1)

else:
  ver = ver_test;
  ver_file = make_filename_ver(ver_test)

###########################################################
# Clean

chdir('..')

package_files = (
  r'ac3filter_%s.exe'      % ver_file,
  r'ac3filter_%s_lite.exe' % ver_file,
  r'ac3filter_%s_pdb.zip'  % ver_file,
  r'ac3filter_%s_src.zip'  % ver_file,
)

for file in package_files:
  if exists(file):
    os.remove(file)

run_at('ac3filter', 'clean.cmd')
run_at('valib', 'clean.cmd')

###########################################################
# Make source archive

src_arc = 'ac3filter_%s_src.zip' % ver_file
src_files = (
  'ac3filter/*.*',
  'valib/*.*',
)
run('%s -add -lev=9 -rec -dir -excl=.hg "%s" %s' % (pkzip, src_arc, ' '.join(src_files)))

###########################################################
# Build valib and run tests

if not exists('samples/test/'):
  raise Exception('No test samples!')

if not exists('samples/test/a.aac.03f.adts'):
  run_at('samples/test', '_build.bat')

chdir('valib/test2')
run('build.cmd release')
run('build.cmd x64 release')
run('test.cmd release')
run('test.cmd x64/release')
chdir('../..')

###########################################################
# Build filter

if ver != ver_test:
  update_ac3filter_ver('ac3filter/ac3filter_ver.h', ver)
  update_changelog('ac3filter/changes.txt', ver)

chdir('ac3filter')
projects = ('acm', 'filter', 'intl', 'tools/ac3config', 'tools/spdif_test')
run('cmd\\build_all vc9 Win32 ' + ' '.join(projects));
run('cmd\\build_all vc9 x64 ' + ' '.join(projects));
run_at('lang', 'update.cmd')
run_at('lang', 'build.cmd')
chdir('..')

###########################################################
# Make PDB archive

pdb_arc = 'ac3filter_%s_pdb.zip' % ver_file

pdb_files = (
 'ac3filter/BugTrap/Release/BugTrap.pdb',
 'ac3filter/BugTrap/x64/Release/BugTrap-x64.pdb',
 'ac3filter/filter/Release/ac3filter.pdb',
 'ac3filter/filter/x64/Release/ac3filter64.pdb',
 'ac3filter/intl/Release/ac3filter_intl.pdb',
 'ac3filter/intl/x64/Release/ac3filter64_intl.pdb',
)
run('%s -add -lev=9 "%s" %s' % (pkzip, pdb_arc, ' '.join(pdb_files)))

###########################################################
# Make installer

run_at('ac3filter', '"%s" /Dappver="%s" /o".." /f"ac3filter_%s" ac3filter.iss' % (innosetup, ver, ver_file))
run_at('ac3filter', '"%s" /Dappver="%s" /o".." /f"ac3filter_%s_lite" ac3filter_lite.iss' % (innosetup, ver, ver_file))

###########################################################
# Commit and tag

if ver != ver_test:
  run('hg commit -R ac3filter -m "Update version"')
  run('hg tag -R ac3filter ac3filter_%s' % ver_file)
  run('hg tag -R valib ac3filter_%s' % ver_file)

###########################################################

print "Done! Build time: %02i:%02i:%02i" % sec2hms(time.time() - start_time)
