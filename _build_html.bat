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
my $date = "05.11.2004";
my $download_bin = "http://prdownloads.sourceforge.net/ac3filter/ac3filter_$ver1.exe?download";
my $download_src = "http://prdownloads.sourceforge.net/ac3filter/ac3filter_${ver1}_src.zip?download";
my $bin = "..\\ac3filter_$ver1.exe";
my $src = "..\\ac3filter_${ver1}_src.zip";
my $package = "..\\ac3filter_$ver1";

my $make_bin = "\"c:\\Program Files\\NSIS\\makensis-bz2\" /DVERSION=$ver /DSETUP_FILE=$bin /DSOURCE_DIR=$package ac3filter.nsi";
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
## Prepare documentation files
##

print "Prepairing documentaion files...\n";

my $changelog;
$changelog = join("<br>", read_file("_changes_eng.txt"));
$changelog =~ s/\s(?=(\s))/\&nbsp\;/g;
write_file("$package\\ac3filter_eng.html", grep { s/(\$\w+)/$1/gee + 1 } read_file("doc\\ac3filter_eng.html"));
write_file("$package\\ac3filter_ita.html", grep { s/(\$\w+)/$1/gee + 1 } read_file("doc\\ac3filter_ita.html"));

$changelog = join("<br>", read_file("_changes_rus.txt"));
$changelog =~ s/\s(?=(\s))/\&nbsp\;/g;
write_file("$package\\ac3filter_rus.html", grep { s/(\$\w+)/$1/gee + 1 } read_file("doc\\ac3filter_rus.html"));


###############################################################################

__END__
:endofperl
