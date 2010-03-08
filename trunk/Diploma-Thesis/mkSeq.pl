#!/usr/bin/perl

sub getMin
{
        $min = 100000000.0;
        foreach $item (@_){
                if ($item <= $min){
                        $min = $item;
                }
        }
        return $min;
}

sub getMax
{
        $max = -100000000.0;
        foreach $item (@_){
                if ($item >= $max){
                        $max = $item;
                }
        }
        return $max;
}

my $fCount = 0;
my $path = "plots/";
my $fileName = "seq_red2_render4_data80_3d_";
my $fileExt = ".tikz";
my $file = "";
while ($fCount < 90){
	#print $fileName;
	$file = $path.$fileName.$fCount.$fileExt;
	open INFILE, "+<", "$file" or die $!;
	my @lines = <INFILE>;
	close INFILE;
	$fCount++;
	$file = $path.$fileName.$fCount.$fileExt;
	open OUTFILE, ">", "$file" or die $!;
	#$count = 0;
	foreach (@lines){
		if (/view=\{(\d*)\}\{(\d*)\}/){
			if(eval($1)>=0 && eval($1)<=90){
				my $azimuth = eval($1)+1;
				my $elevation = eval($2);
				if ($azimuth<24){
					$elevation = getMin($elevation+1, 25);
				}
				elsif ($azimuth>66){
					$elevation = getMax($elevation-1, 0);
				}
				$_ =~ s/view=\{\d*\}\{(\d*)\}/view=\{$azimuth\}\{$elevation\}/;
				print "$_";
			}
		}
		print OUTFILE $_;
	#	if ($_ =~ s/view=\{(\d*)\}\{0\}/view=\{eval($1)\}\{0\}/e){
	#
	#        	print "$_ ";
	#	}
	        #if (substr($_,0,1)==$watchIdx){
	}
	close OUTFILE;
	print "Executing ./mkTikz $file ".$fileName.$fCount.".pdf...\n";
	print `./mkTikz $file $fileName.$fCount."pdf"`;
}
