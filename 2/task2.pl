#!/usr/local/bin/perl
#Hi. You need to write "perl task2.pl filename [filename]" to exec this. The result will be in output.txt"
$str=""; #string
$cv=""; #current value
$cumv=""; #current used memory value
open (OUTPUT, ">output.txt");
$,=" " or die $!;
while($file=shift)
{
	open FILE, $file or die "Can't open file.";
	if(($cv=-s ($file))<5242880)
	{
		if($cumv<524288000)
		{
			@content=<FILE> or die "Can't read, sorry.";
			foreach $line(@content)
			{
				$str.=$line or die "Can't read, sorry.";
			}
			close FILE or die "Can't close file.";
			$cumv=$cumv+$cv;
			#print "121\n";
		}
		else {die "Out of memory!"};
	}
	else {die "It's a very big file here."};
	#print $file."\n";
}
$str=~s/\b[^\s]*[^0-9\s][^\s]*\b/ /g or die "Empty";
$str=~s/\s+/ /g or die "Empty";
@_=sort {$a<=>$b} split(' ',$str) or die $!;
open (OUTPUT, ">>output.txt");# or die $!;
print OUTPUT @_ or die $!;
close OUTPUT or die $!;
print "Check out the output.txt file.";