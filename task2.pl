$str="";
open (OUTPUT, ">output.txt") or die $!;
$,=" " or die $!;
while($file=shift)
{
	open FILE, $file or die $!;
	@content = <FILE> or die $!;
	foreach $line(@content)
	{
		$str .= $line or die $!;
	}
	close FILE or die $!;
}
$str =~ s/\b[^\s]*[^0-9\s][^\s]*\b/ /g or die $!;
$str =~ s/\s+/ /g or die $!;
@_ = sort {$a <=> $b} split(' ', $str) or die $!;
open (OUTPUT, ">>output.txt") or die $!;
print OUTPUT @_ or die $!;
close OUTPUT or die $!;