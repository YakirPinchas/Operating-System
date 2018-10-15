#!/bin/bash

#Student Name:Yakir pinchas
#id:203200530

#the path that user enter
directionary=$(printf %q "$1")

#counter of the files with ".txt"
amount=0

eval cd $directionary

for j in $(ls)
do
	#check the 4 chacthers in the end of the file name.
 	if [[ ${j: -4} == ".txt" ]]
       	then
        #if found , update the counter
		amount=$[$amount + 1]		
        fi
done
echo ‫‪Number‬‬ ‫‪of‬‬ ‫‪files‬‬ ‫‪in‬‬ ‫‪the‬‬ ‫‪directory‬‬ ‫‪that‬‬ ‫‪end‬‬ ‫‪with‬‬ ‫‪.txt‬‬ ‫‪is "$amount"
 






