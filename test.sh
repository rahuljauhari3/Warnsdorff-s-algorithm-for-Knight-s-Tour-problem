#! /usr/bin/bash


#(time ./knight.out 20 0 0) > outputs/0.txt 2>&1

rm -r outputs
mkdir outputs
rm __resultNotSolved.txt

for (( i=0; i<$1; i++ ))
do 
	for (( j=0; j<$1; j++ ))
	do
		(time ./knight.out $1 $i $j) > outputs/[$i][$j].txt 2>&1
	done
done



for (( i=0; i<$1; i++ ))
do 
	for (( j=0; j<$1; j++ ))
	do
		if grep -q "No Possible Tour" outputs/[$i][$j].txt; then
			echo "No solution for (N, StartX, StartY) : ($1, $i, $j)"
			echo "No solution for (N, StartX, StartY) : ($1, $i, $j)" >> __resultNotSolved.txt
		fi
	done
done

