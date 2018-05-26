#!/bin/bash

#opens program, enters input file, outputs to output file,
#checks for differences between output and expected output, then quits

#$1 is the client number. used to find correct input file
echo hello from client_reader$1
# If number of arguments less then 1; print usage and exit

#for testing
if [ $# -lt 1 ]; then
    printf "Usage: %s <reader number>\n" "$0" >&2
    exit 1
fi

bin="./../../build/ui 127.0.0.1" #program to be executed by reader
diff="diff -ad"   # Diff command, to check difference in output vs expected


#declare files
file_in=$"input_reader$1.txt"
file_out_val="output_reader$1.txt"
file_out_expected="expected_output_reader$1.txt"

#ensure it works, remove later
echo my working files are:
echo input: $file_in
echo output: $file_out_val
echo expected output: $file_out_expected
echo


# Validate infile exists (do the same for out validate file)
if [ ! -f "$file_in" ]; then
    printf "In file %s is missing\n" "$file_in"
fi
#validate expected_output_writer exists
if [ ! -f "$file_out_val" ]; then
    printf "Validation file %s is missing\n" "$file_out_val"
fi
#create outputfile
if [ ! -f "$file_out_val" ]; then
    printf "creating output file %s\n" "$file_out_val"
    touch output_writer$1.txt
fi

printf "Testing %s against %s\n" "$file_out_val" "$file_out_expected"

# Run application, redirect in file to app, and output to out file
"$bin" < "$file_in" > "$file_out_val"

# Execute diff
$diff "$file_out_expected" "$file_out_val"


# Check exit code of diff
# adding this to a variable to be able to print it
# Iff not 0 then the files differ (at least with diff)
e_code=$?
if [ $e_code != 0 ]; then
        printf "TEST FAIL : %d\n" "$e_code"
        touch reader_exit.txt
        echo failed with: $e_code >> exit.txt
else
        printf "TEST OK!\n return 0\n"
fi

# Clean exit
exit 0

