#!/bin/bash

#Student Name:Yakir pinchas
#id:203200530

#the first argument from the user
person=$1
#the secend argument from the user
directory=$(echo $2)
#check if the user enter two arguments
if [ "$#" -ne 2 ]
	#print error message
	then echo "error: only two arguments are allowed"
else
        #print the lines in the file with the name that enter
	grep "$person" "$directory"
	#sum the balance of the name that enter in the file and print it in the end
	grep "$person" "$directory" | awk -F ' ' '{sum=sum+$3} END { print "Total balance: "sum}'
fi
