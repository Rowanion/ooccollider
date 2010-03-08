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
my $path = "images/";
my $fileName = "seq_red2_render4_data80_3d_.";
my $fileExt = ".pdf";
my $file = "";
while ($fCount <= 90){
	$inFile = $path.$fileName.$fCount.$fileExt;
	$outFile = "pngs/".$fileName.$fCount.".jpg";
	`convert -density 600x600 -resize 1000x1000 -quality 90 -type truecolor -render $inFile $outFile`;
	$fCount++;
}
