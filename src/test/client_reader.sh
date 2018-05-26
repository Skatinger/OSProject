#!/bin/bash

# change to project directory for execution

cd ..
cd ..

#$1 is the client number. used to find correct input file
# If number of arguments less then 1; print usage and exit
if [ $# -lt 1 ]; then
    exit 1
fi

bin="build/ui 127.0.0.1" #program to be executed by reader
diff="diff -ad"   # Diff command, to check difference in output vs expected


#declare files
file_in=$"src/test/input_reader$1.txt"
file_out_val="src/test/output_reader$1.txt"
file_out_expected="src/test/expected_output_reader$1.txt"

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
    touch src/test/output_reader$1.txt
fi

printf "Testing %s against %s\n" "$file_out_val" "$file_out_expected"

# Run application, redirect in file to app, and output to out file
./build/ui 127.0.0.1 < "$file_in" > "$file_out_val"

# Execute diff
$diff "$file_out_expected" "$file_out_val"


# Check exit code of diff
# adding this to a variable to be able to print it
# Iff not 0 then the files differ (at least with diff)
# python script will evaluate exit files
e_code=$?
if [ $e_code != 0 ]; then
        printf "TEST FAIL : %d\n" "$e_code"
        touch src/test/reader_exit.txt
        echo failed with: $e_code >> src/test/reader_exit.txt
else
        printf "TEST OK!\n return 0\n"
        touch src/test/reader_exit.txt
        echo success >> src/test/reader_exit.txt
fi

# Clean exit
exit 0
