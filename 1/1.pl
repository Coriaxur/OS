use Switch;
use Encode qw / from_to /;

# В соответствии с таблицой:
# $IP = ИП; $COP = КОП; $CHIST = ЧИСТ; $F = Ф;
# $OP = ОП; $PEREH = ПЕРЕХ; 
# $VIB = ВЫБ; $ZAM1 = зам1; $ZAM2 = зам2;
# ZAPP = запп; $PUSK = пуск; $VZAP1 = взап1
# $PRZNK = ПРЗНК и $PR = ПР - признаки результата;
# $IR - индекс-регистр;
# $IA - исполнительный адрес;
# $I = И - поле источника;
# $A = А - адресная часть команды;
# $SP - содержимое ячейки памяти;
# $SUM - содержимое РОН;
# $P - поле приемника;

sub is_Command
{
	$COP = shift @_;
	$PRZ = shift @_;
	$F = shift @_;
	@reciever=split(//,$PRZ);
	$I=0;
	switch($COP) 
	{
		case "00" {$PEREH=0;$OP=0;$P=0;}
		case "01" {$PEREH=0;$OP=1;$P=1;$I=0;}
		case "15" {$PEREH=0;$OP=1;$P=1;$I=1;}
		case "02" {$PEREH=0;$OP=0;$P=2;}
		case "21" {$PEREH=0;$OP=2;$P=1;$I=0;}
		case "25" {$PEREH=0;$OP=2;$P=1;$I=1;}
		case "31" {$PEREH=0;$OP=3;$P=1;$I=0;}
		case "FE" {$PEREH=1;$OP="F";$P=4;}
		case "F0" 
		{
			($reciever[0]==0)?{$PEREH=1}:{$PEREH=0};
			$OP="F";
			$P=4;
		}
		case "F1" 
		{
			($reciever[1]==1)?{$PEREH=1}:{$PEREH=0};
			$OP="F"; 
			$P=4;
		}
		case "F4" 
		{
			($F==0)?{$PEREH=1}:{$PEREH=0}; 
			$OP="F"; 
			$P=4;
		}
		case "F5" 
		{
			($F==1)?{$PEREH=1}:{$PEREH=0}; 
			$OP="F"; 
			$P=4;
		}
		case "FF" {$OP="F";$P=4;}
	}
	($COP eq "FF")?{$PUSK=0}:{$PUSK=1};
	($P==3)?{$VZAP1=1}:{$VZAP1=0};
	($P==1)?{$ZAM1=1}:{$ZAM1=0};
	($P!=3)?{$ZAM2=1}:{$ZAM2=0};
	(($P==2)||($P==3))?{$CHIST=0}:{$CHIST=1};
	$VIB=$I;
	($P==0)?{$ZAPP=1}:{$ZAPP=0};
	return $PUSK, $VZAP1, $ZAM1, $ZAM2, $CHIST, $OP, $VIB, $ZAPP, $PEREH;
}

open (HNDL, 'mem.txt') or die ("Can't read memory");  
$_="";

$i=$F=0;
until (eof(HNDL))
{
	read (HNDL, $x, 1);
	if($F!=0)  {	
					$F=0;
					$readed_mem[$i]=$readed_mem[$i].$x;
					$i++;
				}	
	else
				{
					$F++;
					$readed_mem[$i]=$x;
				}
}
close HNDL;

$i=1;
$size=@readed_mem;
while($i<$size)
{
	$readed_mem[$i]=hex($readed_mem[$i]);
	$i+=2;
}
$IP=$IR=$SUM=$flag=$VNESH=0;
$PRZNK="00";
$COP=$readed_mem[$IP];
$A=$readed_mem[$IP+1];
@mem_state=is_Command($COP, $PRZNK, $flag);
$table="ИП   КОП  А   ПЕРЕХ  ИР  ПРЗНК  ЧИСТ  ИП+1 \n";
from_to ($table,'cp1251','cp866');
print $table;
while($mem_state[0]!=0)
{
	print $IP;
	(length($IP)<2)?{print "    "}
				   :{print "   "};
	$IA = $A+$IR;
	$SP=$readed_mem[$IA];
	if($mem_state[6]==0){$VAM=$SP};
	if($mem_state[6]==1){$VAM=$IA};
	if($mem_state[6]==2){$VAM=$VNESH};
	$RESULT=0;
	$PR="00";
	($mem_state[5]==0)?{$RESULT=$SUM}:{};
	($mem_state[5]==1)?{$RESULT=$VAM}:{};
	($mem_state[5]==2)?{$RESULT=$SUM+$VAM}:{};
	($mem_state[5]==3)?{$RESULT=$SUM-$VAM}:{};
	($RESULT==0)?{$PR="0"}:{$PR="1"};
	($RESULT>0)?{$PR.="1"}:{$PR.="0"};
	if($mem_state[2]==1)
	{
		$PRZNK=$PR;
		$SUM=$RESULT;
	}
	if($mem_state[3]==1)
	{
		if($mem_state[4]==0){$IR=$RESULT};
		if($mem_state[4]==1){$IR=0};
	}
	if($mem_state[7]==1){$readed_mem[$IA]=$RESULT};
	if($mem_state[1]==1)
	{
		$flag=1;
		$VNESH=$RESULT;
	}
	$IP+=2;
	if($mem_state[8]==0){$IP=$IP};
	if($mem_state[8]==1){$IP=$IA};
	$COP=$readed_mem[$IP];
	$A=$readed_mem[$IP+1];
	@mem_state=is_Command($COP, $PRZNK, $flag);
	print $COP."   ";
	print $A;
	(length($A)<2)?{print "   "}
				   :{print "  "};
	print $PEREH."      ".$IR."   ".$PRZNK."     ".$CHIST."     ".$IP."\n";
}