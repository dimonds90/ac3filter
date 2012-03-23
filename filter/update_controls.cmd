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
# build_controls
# --------------
#
# This script scans the resource file and builds a full list of controls that
# can be compiled with a program.
#
# The main reason of the generated file is to bind control id, label and tip
# (we cannot specify the tip text directly at the resource script) and get
# a way to translate all this.
#
# Controls file is used to translate dialog controls, create and translate tips
# (or revert translation).
#
# Later controls file (with strings file) is used to generate PO Template file
#
#      resource script
#            |
#            V
#     (build_controls)
#            |
#            V
#      controls list <---- tips (manual edit of controls file)
#            |
#            V
#        (gettext) <---- other source code
#            |
#            V
#       PO Template
#
#
# TODO:
# * list of dialogs
# * list of controls per dialog
# * do not remove unused controls from the original file during update

use strict;

my $res_file = "ac3filter.rc";
my $controls_file = "dialog_controls.h";



###########################################################
#
# PARSE RESOURCE FILE
#
###########################################################



my %id_type;  # control id -> control type
my %id_label; # control id -> label text
my %id_group; # control id -> group id
my %groups;   # list of groups

open(RES, "< $res_file") || die "Cannot open file $res_file";
my $res = join("", <RES>);
close(RES);

foreach my $block ($res =~ /^BEGIN$(.*?)^END$/gsm)
{
  #print ">>> BEGIN\n";

  ###########################################################
  # Dialog-wide data

  my %grp_x1;
  my %grp_y1;
  my %grp_x2;
  my %grp_y2;
  my %id_x;
  my %id_y;

  ###########################################################
  # Populate controls array

  my @controls = ();
  push(@controls, $block =~ /GROUPBOX\s+"[^"]*",\s*\w+,\s*\d+,\s*\d+,\s*\d+,\s*\d+/gs);
  push(@controls, $block =~ /(?:LTEXT|CTEXT|RTEXT)\s+"[^"]*",\s*\w+,\s*\d+,\s*\d+,\s*\d+,\s*\d+/gs);
  push(@controls, $block =~ /(?:PUSHBUTTON|RADIOBUTTON)\s+"[^"]*",\s*\w+,\s*\d+,\s*\d+,\s*\d+,\s*\d+/gs);
  push(@controls, $block =~ /(?:EDITTEXT|COMBOBOX)\s+\w+,\s*\d+,\s*\d+,\s*\d+,\s*\d+/gs);
  push(@controls, $block =~ /CONTROL\s+"[^"]*",\s*\w+,\s*"[^"]*",.*?,\s*\d+,\s*\d+,\s*\d+,\s*\d+/gs);

  ###########################################################
  # Parse controls

  foreach (sort @controls)
  {
    my ($type, $id, $label, $x, $y, $w, $h);
    (($type, $label, $id, $x, $y, $w, $h) = /(GROUPBOX)\s+"([^"]*)",\s*(\w+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+)/gs) ||
    (($type, $label, $id, $x, $y) = /(LTEXT|CTEXT|RTEXT)\s+"([^"]*)",\s*(\w+),\s*(\d+),\s*(\d+)/gs) ||
    (($type, $label, $id, $x, $y) = /(PUSHBUTTON|RADIOBUTTON)\s+"([^"]*)",\s*(\w+),\s*(\d+),\s*(\d+)/gs) ||
    (($type, $label, $id, $x, $y) = /(CONTROL)\s+"([^"]*)",\s*(\w+),\s*"[^"]*",.*?,\s*(\d+),\s*(\d+)/gs) ||
    (($type, $id, $x, $y) = /(EDITTEXT|COMBOBOX)\s+(\w+),\s*(\d+),\s*(\d+)/gs) ||
    print "Strange control $_\n" && next;

    next if ($id eq "IDC_STATIC" || !$label);

    undef $label if ($type eq "CONTROL" && $label eq "Slider1");   # not a label...
    undef $label if ($type eq "CONTROL" && $label eq "Slider3");   # not a label...
    undef $label if ($type eq "CONTROL" && $label eq "Progress1"); # not a label...

    print "Control $id has different types $id_type{$id} and $type\n" if ($id_type{$id} && $id_type{$id} ne $type);
    print "Control $id has different labels \"$id_label{$id}\" and \"$label\"\n" if ($id_label{$id} && $id_label{$id} ne $label);

    $id_type{$id} = $type;
    $id_label{$id} = $label;

    if ($id_type{$id} eq "GROUPBOX")
    {
      $groups{$id}++;
      $grp_x1{$id} = $x;
      $grp_y1{$id} = $y;
      $grp_x2{$id} = $x+$w;
      $grp_y2{$id} = $y+$h;
    }
    else
    {
      $id_x{$id} = $x;
      $id_y{$id} = $y;
    }
  }

  ###########################################################
  # Assign groups to controls

  foreach my $id (sort keys %id_x)
  {
    my $found = 0;
    foreach my $grp (keys %grp_x1)
    {
      if ($id_x{$id} > $grp_x1{$grp} && $id_x{$id} < $grp_x2{$grp} &&
          $id_y{$id} > $grp_y1{$grp} && $id_y{$id} < $grp_y2{$grp})
      {
        $found++;    
        #print "Control $id belongs to groups $id_group{$id} and $grp\n" if ($id_group{$id} && $id_group{$id} ne $grp)
        $id_group{$id} = $grp if (!$id_group{$id})
      }
    }
    #print "Control $id does not belong to any group\n" if (!$found);
  }
}



###########################################################
#
# PARSE CONTROLS FILE
#
###########################################################



my %ctrl_id;       # id exists
my %ctrl_label;    # label
my %ctrl_tip;      # tip text

###########################################################
# Read controls table

open (CTRL, "< $controls_file");
while (<CTRL>)
{
  # Replace N_("sometext") macro to "sometext"
  s/N_\(("[^"]*")\)/$1/gsx;

  # Parse example:
  # { ID, "ID" , "label text" , "tip text"

  my ($id, $label, $tip);
  (($id, $label, $tip) = /\s*{\s*(\w+)\s*,\s*"[^"]*"\s*,\s*"([^"]*)"\s*,\s*"([^"]*)"/gsx) || next;

  $ctrl_id{$id}++;
  $ctrl_label{$id} = $label;
  $ctrl_tip{$id} = $tip;
}
close(CTRL);

###########################################################
# Find changes

my @removed = grep { !$id_type{$_} } keys %ctrl_id;
my @new = grep { !$ctrl_id{$_} } keys %id_type;
my @renamed = grep { $id_type{$_} && $ctrl_label{$_} ne $id_label{$_} } keys %ctrl_id;

print "Removed controls(",scalar(@removed),"):", join(", ", @removed), "\n";
print "New controls (",scalar(@new),"):", join(", ", @new), "\n";
print "Renamed controls (",scalar(@renamed),"):", join("\n", @renamed), "\n";


###########################################################
#
# WRITE CONTROLS FILE
#
###########################################################



open (CTRL, "> $controls_file");

my $define = uc($controls_file);
$define =~ s/[\s\.]/_/g;
my $groups = scalar(keys %groups);
my $controls = scalar(keys %id_type);

print CTRL <<EOTEXT;
// File is generated by build_controls script from resource script.
// (see comments at the script)

#ifndef $define
#define $define

struct ControlDesc
{
  int id;
  const char *strid;
  const char *label;
  const char *tip;
};

// Total groups: $groups
// Total controls: $controls
static const ControlDesc dialog_controls[] =
{
EOTEXT

print CTRL "  // Controls without a group\n";
foreach my $id (sort grep { !$id_group{$_} && !$groups{$_} } keys %id_type)
{
  print CTRL "  /* TRANSLATORS: $id_label{$id} */\n" if ($id_label{$id});
  print CTRL "  { $id, ".format_trans($id, $id_label{$id}).", \"$ctrl_tip{$id}\" },\n";
}

foreach my $grp (sort keys %groups)
{
  print CTRL "\n  // $id_label{$grp}\n";
  print CTRL "  /* TRANSLATORS: $id_label{$grp} */\n" if ($id_label{$grp});
  print CTRL "  { $grp, ".format_trans($grp, $id_label{$grp}).", \"$ctrl_tip{$grp}\" },\n";
  foreach my $id (sort grep { $id_group{$_} eq $grp } keys %id_type)
  {
    print CTRL "  /* TRANSLATORS: $id_label{$id} */\n" if ($id_label{$id});
    print CTRL "  { $id, ".format_trans($id, $id_label{$id}).", \"$ctrl_tip{$id}\" },\n";
  }
}
print CTRL "};\n\n";
print CTRL "#endif\n";
close CTRL;


sub format_trans
{
  my $id = shift;
  my $text = shift;

  return "N_(\"$id\"), \"$text\"" if $text;
  return "\"$id\", \"$text\"";
}

__END__
:endofperl
