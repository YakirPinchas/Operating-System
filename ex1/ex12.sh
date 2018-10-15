#!/bin/bash

#Student Name:Yakir pinchas
#id:203200530

#pass is the file or path that user enter
pass=$(printf %q "$1")

eval cd $pass
#seperate the 'n\' between the files
IFS=$'\n'
#loop files and dictionary
for j in $(ls)
do
	if [[ -d $j ]]; then
                #check if the argument is directory 
    		echo "The name is $j is a directory"
	elif [[ -f $j ]]; then
                #check if the argument is file
    		echo "The name is $j is a file"
	fi
	
done

