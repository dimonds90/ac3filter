@rem = '--*-Perl-*--
@echo off
if "%OS%" == "Windows_NT" goto WinNT
perl -x -S "%0" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
:WinNT
perl -x -S %0 %*
if NOT "%COMSPEC%" == "%SystemRoot%\system32\cmd.exe" goto endofperl
if %errorlevel% == 9009 echo You do not have Perl in your PATH.
if errorlevel 1 goto script_failed_so_exit_with_non_zero_val 2>nul
goto endofperl
@rem ';
#!perl
#line 15
use strict;


###########################################################
# Version

my $ver = shift || "internal";
my $ver1 = $ver;
$ver1 =~ s/[\s\.\\\/]/_/g;



###########################################################
# Other vars

my $download_bin = "http://prdownloads.sourceforge.net/ac3filter/ac3filter_$ver1.exe?download";
my $download_src = "http://prdownloads.sourceforge.net/ac3filter/ac3filter_${ver1}_src.zip?download";
my $package = "..\\ac3filter_$ver1";
my $bin = "..\\ac3filter_$ver1.exe";
my $src = "..\\ac3filter_${ver1}_src.zip";

my $make_bin = "\"c:\\Devel\\NSIS\\makensis\" /NOCONFIG /DVERSION=\"$ver\" /DSETUP_FILE=$bin /DSOURCE_DIR=$package ac3filter.nsi";
my $make_src = "pkzip25 -add -rec -dir -lev=9 $src";



###########################################################
# Some funcitons

sub read_file
{
  my ($filename) = (@_);
  open (FILE, "< $filename") || die "cannot open file $filename";
  my @result = <FILE>;
  close (FILE);
  return @result;
}

sub write_file
{
  my $filename = shift;
  open (FILE, "> $filename") || die "cannot open file $filename";
  print FILE @_;
  close (FILE);
}

sub set_ver
{
  my $ver = shift;
  open VER, "> ac3filter_ver.h";
  printf VER <<EOVER;
#ifndef AC3FILTER_VER_H
#define AC3FILTER_VER_H

#define AC3FILTER_VER "$ver"

#endif
EOVER
  close VER;
}

sub build_project
{
  my $dir = shift;
  my $project = shift;
  system("msdev $dir\\$project.dsp /MAKE \"$project - Win32 Release\" /REBUILD")
    && die "failed!!!";
  `_clear.bat`;
}



###########################################################
# Build projects

print "Building project...\n";

set_ver($ver);
build_project('chineese_patch', 'chineese_patch');
build_project('ac3config', 'ac3config');
build_project('.', 'ac3filter');
`_clear.bat`;
set_ver("internal");



###############################################################################
# Build package

printf "Building package...\n";
`rmdir /s /q $package 2> nul`;
`mkdir $package`;

`copy chineese_patch\\release\\chineese_patch.exe $package\\dialog_patch.exe`;
`copy ac3config\\release\\ac3config.exe $package\\ac3config.exe`;
`copy release\\ac3filter.ax $package`;

`copy _readme.txt $package`;
`copy GPL_eng.txt $package`;
`copy GPL_rus.txt $package`;



###############################################################################
## Make distributive

`del $bin 2> nul`;
system($make_bin);

__END__
:endofperl
