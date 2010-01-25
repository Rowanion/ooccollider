#!/usr/bin/perl

################################################################
# The script reads a file given on cmd-line and reads it
# in completely. Then it writes it back, adding a counter
# from 0 to i followed by a colon "," at the beginning
# of each line. Lines beginning with '#' are written
# back as they are.
################################################################

$fileName = $ARGV[0];
#print $fileName;
open INFILE, "+<", "$fileName" or die $!;
my @lines = <INFILE>;
close INFILE;
open OUTFILE, "+>", "$fileName" or die $!;
$count = 0;
foreach (@lines){
	#print "$_ ";
	if (substr($_,0,1)!="#"){
		print OUTFILE "$count,".$_;
		$count++;
	}
	else {
		print OUTFILE $_;
	}
}
print OUTFILE "\n";
close OUTFILE;

