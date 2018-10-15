#!/bin/bash

#Student Name:Yakir pinchas
#id:203200530

#the argument from user
file=$(printf %q "$1")
#the file that i want to copy
diff=$(echo $1 | xargs)
#check if the user enter only one argument
if [ "$#" -ne 1 ]
	#print message currectly
	then echo "error: only one argument is allowed"
elif [ -e "$diff" ]
	#check if there is a directory named "safe_rm_dir"
	then if [ ! -d safe_rm_dir ]
		#make the directory
        	then mkdir safe_rm_dir
	fi
	#pass a copy of the file to the directory
        cp "$diff" safe_rm_dir/
	#delete the file
        rm "$diff"
	#print
        echo "done!"
else
	#print - if the file donwt exist
        echo "error: there is no such file"
fi


































