@rem = '--*-Perl Script Wrapper-*--';
@rem = '
@echo off
set CMD=%0
set ARGS=
:loop
if .%1==. goto endloop
set ARGS=%ARGS% %1
shift
goto loop
:endloop
perl.exe -w -S %CMD% %ARGS%
goto endofperl
@rem ';

###############################################################################

use strict;

my $ver  = "1.01a";
my $ver1 = "1_01a";
my $date = "01.06.2004";
my $download_bin = "http://prdownloads.sourceforge.net/ac3filter/ac3filter_$ver1.exe?download";
my $download_src = "http://prdownloads.sourceforge.net/ac3filter/ac3filter_${ver1}_src.zip?download";
my $bin = "..\\ac3filter_$ver1.exe";
my $src = "..\\ac3filter_${ver1}_src.zip";
my $package = "..\\ac3filter_$ver1";

my $make_bin = "\"c:\\Devel\\NSIS\\makensis-bz2\" /DVERSION=$ver /DSETUP_FILE=$bin /DSOURCE_DIR=$package ac3filter.nsi";
my $make_src = "pkzip25 -add -rec -dir -lev=9 $src";

#####!packhdr tmp.dat "upx --best tmp.dat" - Add it to ac3filter.nsi?

sub read_file
{
  my ($filename) = (@_);
  open (FILE, "< $filename");
  my @result = <FILE>;
  close (FILE);
  return @result;
}

sub write_file
{
  my $filename = shift;
  open (FILE, "> $filename");
  print FILE @_;
  close (FILE);
}

###############################################################################
##
## Clean
##

printf "Cleaning...\n";
`rmdir /s /q $package 2> nul`;
`mkdir $package`;


###############################################################################
##
## Build project
##

print "Building project...\n";

system('msdev ac3filter.dsp /MAKE "ac3filter - Win32 Release" /REBUILD')
  && die "failed!!!";

`copy release\\ac3filter.ax $package`;
`rmdir /s /q debug   2> nul`;
`rmdir /s /q release 2> nul`;


system('msdev chineese_patch\\chineese_patch.dsp /MAKE "chineese_patch - Win32 Release" /REBUILD')
  && die "failed!!!";

`copy chineese_patch\\release\\chineese_patch.exe $package\\dialog_patch.exe`;
`rmdir /s /q chineese_patch\\debug 2> nul`;
`rmdir /s /q chineese_patch\\release 2> nul`;


system('msdev ac3config\\ac3config.dsp /MAKE "ac3config - Win32 Release" /REBUILD')
  && die "failed!!!";

`copy ac3config\\release\\ac3config.exe $package\\ac3config.exe`;
`rmdir /s /q ac3config\\debug 2> nul`;
`rmdir /s /q ac3config\\release 2> nul`;

`_clear.bat`;

###############################################################################
##
## Prepare documentation files
##

print "Prepairing documentaion files...\n";

my $changelog = join("<br>", read_file("_changes_eng.txt"));
$changelog =~ s/\s(?=(\s))/\&nbsp\;/g;
write_file("$package\\ac3filter_eng.html", grep { s/(\$\w+)/$1/gee + 1 } read_file("doc\\ac3filter_eng.html"));
write_file("$package\\ac3filter_ita.html", grep { s/(\$\w+)/$1/gee + 1 } read_file("doc\\ac3filter_ita.html"));

my $changelog = join("<br>", read_file("_changes_rus.txt"));
$changelog =~ s/\s(?=(\s))/\&nbsp\;/g;
write_file("$package\\ac3filter_rus.html", grep { s/(\$\w+)/$1/gee + 1 } read_file("doc\\ac3filter_rus.html"));

`mkdir $package\\pic`;
`copy doc\\pic $package\\pic`;

###############################################################################
##
## Prepare documentation files
##

`copy _readme.txt $package`;
`copy GPL_eng.txt $package`;
`copy GPL_rus.txt $package`;


###############################################################################
##
## Make distributives
##

`del $bin 2> nul`;
`del $src 2> nul`;

system($make_src);
system($make_bin);

###############################################################################

__END__
:endofperl
