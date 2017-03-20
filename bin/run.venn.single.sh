#!/bin/bash

if [ "$#" -ne 1 ]
then
	echo "usage $0 dataset"
	exit
fi

sc="scallop.B668.0.01"
st="stringtie.1.3.2d.0.01"
tc="transcomb.0.01"

dir=`pwd`/$1.W668.1.3.2d
mkdir -p $dir

gtf=`pwd`/p2_sorted.gtf
roc=`pwd`/gtfcuff
gtfmerge=`pwd`/gtfmerge

function commgtf()
{
	tmp=./list.xxx.tmp;
	echo $1 > $tmp;
	echo $2 >> $tmp;
	$gtfmerge intersection $tmp $3;
	rm -rf $tmp;
}

ln -sf `pwd`/$1.tophat/$sc/scallop.gtf $dir/scallop.1.gtf
ln -sf `pwd`/$1.star/$sc/scallop.gtf $dir/scallop.2.gtf
ln -sf `pwd`/$1.hisat/$sc/scallop.gtf $dir/scallop.3.gtf

ln -sf `pwd`/$1.tophat/$st/st.gtf $dir/stringtie.1.gtf
ln -sf `pwd`/$1.star/$st/st.gtf $dir/stringtie.2.gtf
ln -sf `pwd`/$1.hisat/$st/st.gtf $dir/stringtie.3.gtf

ln -sf `pwd`/$1.tophat/$tc/TransComb.gtf $dir/transcomb.1.gtf
ln -sf `pwd`/$1.star/$tc/TransComb.gtf $dir/transcomb.2.gtf

cd $dir

for k in `echo "1 2 3"`
do
	gffcompare -r $gtf scallop.$k.gtf -M -N
	refsize=`cat $dir/gffcmp.stats | grep Reference | grep mRNA | awk '{print $5}'`
	$roc split gffcmp.scallop.$k.gtf.tmap scallop.$k.gtf scallop.$k.true.gtf false.gtf
	$roc roc gffcmp.scallop.$k.gtf.tmap $refsize > scallop.$k.roc
	rm gffcmp.* false.gtf
done

for k in `echo "1 2 3"`
do
	gffcompare -r $gtf stringtie.$k.gtf -M -N
	refsize=`cat $dir/gffcmp.stats | grep Reference | grep mRNA | awk '{print $5}'`
	$roc split gffcmp.stringtie.$k.gtf.tmap stringtie.$k.gtf stringtie.$k.true.gtf false.gtf
	$roc roc gffcmp.stringtie.$k.gtf.tmap $refsize > stringtie.$k.roc
	rm gffcmp.* false.gtf
done

for k in `echo "1 2"`
do
	gffcompare -r $gtf transcomb.$k.gtf -M -N
	refsize=`cat $dir/gffcmp.stats | grep Reference | grep mRNA | awk '{print $5}'`
	$roc split gffcmp.transcomb.$k.gtf.tmap transcomb.$k.gtf transcomb.$k.true.gtf false.gtf
	$roc roc gffcmp.transcomb.$k.gtf.tmap $refsize > transcomb.$k.roc
	rm gffcmp.* false.gtf
done

a1="scallop"
a2="stringtie"
a3="transcomb"

# different algorithm
xx=$1
for k in `echo "1 2"`
do
	commgtf $dir/$a1.$k.true.gtf $dir/$a2.$k.true.gtf  $dir/$a1.$a2.$k.true.gtf
	commgtf $dir/$a1.$k.true.gtf $dir/$a3.$k.true.gtf  $dir/$a1.$a3.$k.true.gtf
	commgtf $dir/$a2.$k.true.gtf $dir/$a3.$k.true.gtf  $dir/$a2.$a3.$k.true.gtf
	commgtf $dir/$a1.$a2.$k.true.gtf $dir/$a3.$k.true.gtf  $dir/$a1.$a2.$a3.$k.true.gtf

	xa=`cat $dir/$a1.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	xb=`cat $dir/$a2.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	xc=`cat $dir/$a3.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	x1=`cat $dir/$a1.$a2.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	x2=`cat $dir/$a1.$a3.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	x3=`cat $dir/$a2.$a3.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	x4=`cat $dir/$a1.$a2.$a3.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	xx="$xx $xa $xb $xc $x1 $x2 $x3 $x4"
done

for k in `echo "3"`
do
	commgtf $dir/$a1.$k.true.gtf $dir/$a2.$k.true.gtf  $dir/$a1.$a2.$k.true.gtf
	xa=`cat $dir/$a1.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	xb=`cat $dir/$a2.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	x1=`cat $dir/$a1.$a2.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	xx="$xx $xa $xb $x1"
done

echo $xx > $dir/algo.summary

# different aligner
cd $dir
commgtf $dir/scallop.1.true.gtf $dir/scallop.2.true.gtf  $dir/scallop.12.true.gtf
commgtf $dir/scallop.1.true.gtf $dir/scallop.3.true.gtf $dir/scallop.13.true.gtf
commgtf $dir/scallop.2.true.gtf $dir/scallop.3.true.gtf $dir/scallop.23.true.gtf
commgtf $dir/scallop.1.true.gtf $dir/scallop.23.true.gtf $dir/scallop.123.true.gtf

commgtf $dir/stringtie.1.true.gtf $dir/stringtie.2.true.gtf $dir/stringtie.12.true.gtf
commgtf $dir/stringtie.1.true.gtf $dir/stringtie.3.true.gtf $dir/stringtie.13.true.gtf
commgtf $dir/stringtie.2.true.gtf $dir/stringtie.3.true.gtf $dir/stringtie.23.true.gtf
commgtf $dir/stringtie.1.true.gtf $dir/stringtie.23.true.gtf $dir/stringtie.123.true.gtf

commgtf $dir/transcomb.1.true.gtf $dir/transcomb.2.true.gtf $dir/transcomb.12.true.gtf

xx=$1
for k in `echo "1 2 3 12 13 23 123"`
do
	x=`cat scallop.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	xx="$xx $x"
done

for k in `echo "1 2 3 12 13 23 123"`
do
	x=`cat stringtie.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	xx="$xx $x"
done

for k in `echo "1 2 12"`
do
	x=`cat transcomb.$k.true.gtf | awk '$3 == "transcript"' | wc -l`
	xx="$xx $x"
done

echo $xx > $dir/aligner.summary
