import os, sys, re, time, argparse
from os import chdir
from os.path import exists

ver_test = 'test'

if not exists('config.py'):
  raise('Copy config.py.example to config.py and adjust configuration')
from config import *

###########################################################
# Command line parser

parser = argparse.ArgumentParser(description='Build distributive.')

parser.add_argument('ver',
  nargs='?',
  default=ver_test,
  help='Distributive version.')

parser.add_argument('--no-clean',
  dest='clean',
  action='store_false',
  help='Do not clean before build.')

parser.add_argument('--no-source',
  dest='source',
  action='store_false',
  help='Do not create source archive.')

parser.add_argument('--no-symbols',
  dest='symbols',
  action='store_false',
  help='Do not create symbols archive.')

parser.add_argument('--no-test',
  dest='test',
  action='store_false',
  help='Do not run tests.')

parser.add_argument('--no-opencandy',
  dest='opencandy',
  action='store_false',
  help='Do not bundle OpenCandy.')

args = parser.parse_args()

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
  m = re.match(r'(\d+)\.(\d+)\.(\d+)(.?)', ver)
  if not m:
    raise Exception('Incorrect version format.')
  return m.groups()  

def make_filename_ver(ver_dot):
  return ver_dot.replace('.', '_')

def update_ac3filter_ver(file, ver):
  result = []
  major, minor, release, state = split_ver(ver)
  for str in open(file):
    if str.find('#define AC3FILTER_VER_MAJOR') == 0:
      result.append('#define AC3FILTER_VER_MAJOR %s\n' % major)
    elif str.find('#define AC3FILTER_VER_MINOR') == 0:
      result.append('#define AC3FILTER_VER_MINOR %s\n' % minor)
    elif str.find('#define AC3FILTER_VER_RELEASE') == 0:
      result.append('#define AC3FILTER_VER_RELEASE %s\n' % release)
    elif str.find('#define AC3FILTER_VER_STATE') == 0:
      result.append('#define AC3FILTER_VER_STATE "%s"\n' % state)
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

if args.ver != ver_test:
  ver = args.ver
  ver_file = make_filename_ver(ver)

  if args.opencandy and not exists('OCKeys.iss'):
    ans = raw_input('OpenCandy keys file (OCKeys.iss) not found. Continue without OpenCandy integration? (Y/n)?'.format(ver));
    if ans != 'Y':
      sys.exit(1)

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
  r'ac3filter_%s_sym.zip'  % ver_file,
  r'ac3filter_%s_src.zip'  % ver_file,
)

for file in package_files:
  if exists(file):
    os.remove(file)

if args.clean:
  run_at('ac3filter', 'clean.cmd')
  run_at('valib', 'clean.cmd')

###########################################################
# Make source archive

src_arc = 'ac3filter_%s_src.zip' % ver_file
src_files = (
  'ac3filter/*.*',
  'valib/*.*',
)

if args.source:
  run('%s -add -lev=9 -rec -dir -excl=.hg "%s" %s' % (pkzip, src_arc, ' '.join(src_files)))

###########################################################
# Build valib and run tests

if args.test:
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
# Make symbols archive

sym_arc = 'ac3filter_%s_sym.zip' % ver_file

sym_files = (
 'ac3filter/acm/Release/ac3filter.acm',
 'ac3filter/acm/Release/ac3filter.pdb',
 'ac3filter/acm/x64/Release/ac3filter64.acm',
 'ac3filter/acm/x64/Release/ac3filter64.pdb',
 'ac3filter/BugTrap/Release/BugTrap.dll',
 'ac3filter/BugTrap/Release/BugTrap.pdb',
 'ac3filter/BugTrap/x64/Release/BugTrap-x64.dll',
 'ac3filter/BugTrap/x64/Release/BugTrap-x64.pdb',
 'ac3filter/filter/Release/ac3filter.ax',
 'ac3filter/filter/Release/ac3filter.pdb',
 'ac3filter/filter/x64/Release/ac3filter64.ax',
 'ac3filter/filter/x64/Release/ac3filter64.pdb',
 'ac3filter/intl/Release/ac3filter_intl.dll',
 'ac3filter/intl/Release/ac3filter_intl.pdb',
 'ac3filter/intl/x64/Release/ac3filter64_intl.dll',
 'ac3filter/intl/x64/Release/ac3filter64_intl.pdb',
)

for f in sym_files:
  if not exists(f):
    raise Exception('Symbol file not found: "{}"'.format(f))

if args.symbols:
  run('%s -add -rec -dir -lev=9 "%s" %s' % (pkzip, sym_arc, ' '.join(sym_files)))

###########################################################
# Make installer

iss_defines = []
iss_defines.append('/Dappver="%s"' % ver)
if exists('ac3filter/OCKeys.iss') and args.opencandy:
  iss_defines.append('/DOPENCANDY')

iss_defines = ' '.join(iss_defines)
run_at('ac3filter', '"%s" %s /o".." ac3filter.iss' % (innosetup, iss_defines))
run_at('ac3filter', '"%s" %s /o".." ac3filter_lite.iss' % (innosetup, iss_defines))

###########################################################
# Commit and tag

if ver != ver_test:
  run('hg tag -R ac3filter ac3filter_%s' % ver_file)
  run('hg tag -R valib ac3filter_%s' % ver_file)
  run('hg commit -R ac3filter -m "Update version"')

###########################################################

print "Done! Build time: %02i:%02i:%02i" % sec2hms(time.time() - start_time)
