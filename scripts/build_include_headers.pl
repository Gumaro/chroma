#!/usr/bin/perl
#
# $Id: build_include_headers.pl,v 1.1 2004-01-08 03:13:49 edwards Exp $
#
# Build the  nobase_include_headers  lines in  chroma/lib/Makefile.am
#
# Usage
#   cd chroma/lib
#   build_include_headers.pl
#

print "nobase_include_HEADERS =";
# Top level include files
# NOTE: no Wilson or staggered specific here
system('ls *.h | fmt -w 65 | awk \'{printf " \\\\\\n\t%s", $0}\'');

# All subdirs
@subdirs = ("actions", "geom", "info", "io", "meas", "util");

# Go through each dir and find fermion independent headers
foreach $i (@subdirs)
{
  system("find $i -name \"*.h\" -print | grep -v '_[ws].h' | fmt -w 65 | awk '{printf \" \\\\\\n\\t%s\", \$0}'");
}

print "\n\n";

# Now print only the Wilson specific headers
print "if BUILD_WILSON_LIBS\n";
print "nobase_include_HEADERS +=";

# Go through each dir and find Wilson specific headers
foreach $i (@subdirs)
{
  system("find $i -name \"*_w.h\" -print | fmt -w 65 | awk '{printf \" \\\\\\n\\t%s\", \$0}'");
}

print "\nendif\n\n";

# Now print only the Staggered specific headers
print "if BUILD_STAGGERED_LIBS\n";
print "nobase_include_HEADERS +=";

# Go through each dir and find Staggered specific headers
foreach $i (@subdirs)
{
  system("find $i -name \"*_s.h\" -print | fmt -w 65 | awk '{printf \" \\\\\\n\\t%s\", \$0}'");
}

print "\nendif\n";


exit(0);
