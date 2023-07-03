#!/bin/bash

# TO USE THIS TEST SCRIPT

# FIRST USE ONLY - in terminal type: 
# chmod +x test.sh
# this ensures you have permission to run this script
# you should only need to do this once.



# You can then run with: 
# ./test.sh
# You can save your tests into a file using:
# ./tests.sh > name_of_file.txt
# you can edit which executables to run on line 90.

# checking for the -echo option.
ECHO=0
if [[ $1 = '-echo' ]]
then
    ECHO=1
fi

# we clean up object files and then recompile as standard
# this is because when you may be working across multiple devices
# your object files are not necessarily tranferable, and therefore
# you should always recompile.
make clean
make all

######### DO NOT EDIT THIS SECTION #########
# this section contains various things which the rest of the code relies on.
# If you want to edit anything here:
#       1) make a backup of this file first
#       2) read the comments and ensure you understand what you are changing


# setting up correct files for permission tests
# touch creates a new file (if it doesn't already exist)
# and chmod is used to set permissons
# -w means 'remove write permissions for all users'
# -r means 'remove read permissions for all users'
# we restore these at the end of the tests.
touch tests/data/bad.out
chmod -w tests/data/bad.out
chmod -r tests/data/ebf_data/bad_perms.ebf
chmod -r tests/data/ebu_data/bad_perms.ebu
chmod -r tests/data/ebc_data/bad_perms.ebc

# keep track of score and total count of tests
total=0
score=0

# Usage: run_test $1 $2 $3 $4 $5
# Args: $1=executable name $2=executable arg 1 $3=executable arg 2 $4=expected return value $5=expected message

# This function takes in the arguments stated above and runs the given executable
# with the two specified arguments.
# the output code and string are captured
# and these are compared to the given expected code and string
# displaying a helpful message to the user
# and updating both the number of tests run and the number of passes
run_test () 
    { # run_test()

    if [ $ECHO -eq 1 ]
    then
        echo $1 $2 $3
    fi

    # capture returned message
    message=$($1 $2 $3) 
    # run again (pipe to null so it doesn't display to user) for the output code
    $1 $2 $3 > null 
    out=$?
    # increment total by 2 because 2 tests are run (code and message)
    total=$(( $total+2 ))

    # check output value against expected
    if [ $out == $4 ]
    then
        echo "PASSED: expected $4 got $out"
        # if we passed, increment the score
        (( score++ ))
    else
        echo "FAILED: expected $4 got $out"
    fi

    # check output message against expected
    if [ "$message" == "$5" ] 
    then
        echo "PASSED: expected $5 got $message"
        (( score++ ))
    else
        echo "FAILED: expected $5 got $message"
    fi

    } # end run_test()

######### YOU CAN EDIT BELOW THIS LINE #########



# you can remove or comment out any executables you don't want to test
# full list of executables: ebf2ebu ebuEcho ebuComp ebu2ebf
EXES=(ebfEcho ebfComp ebuComp ebuEcho ebf2ebu ebu2ebf ebcComp ebcEcho ebc2ebu ebu2ebc)

# run all of the tests below for all executables given in 'EXES'
# inside this loop, the executable being run can be referred to by 'testExecutable'
for testExecutable in ${EXES[@]}
do
    # check the first 3 letters of the executable's name - this is used to
    # set up the correct file extension and path for use in tests.
    if [[ ${testExecutable::3} == "ebf" ]]
    then
        file_ext=".ebf"
        path="tests/data/ebf_data/"
    elif [[ ${testExecutable::3} == "ebc" ]]
    then
        file_ext=".ebc"
        path="tests/data/ebc_data/"
    else
        file_ext=".ebu"
        path="tests/data/ebu_data/"
    fi
    echo "-------------- TESTING $testExecutable --------------"

    # The order of these tests is based on when things should fail
    # this order is important!
    # For example, if I run ./ebfEcho test.ebf
    # I should never get as far as opening the file since there is
    # and incorrect number of arguments.
    # Opening the file would be a waste of processing time and memory
    # because the command is invalid.

    # In many later tests, the 2nd argument may not exist.
    # You may find that you are getting 'Bad File Name (tmp)' if you have structured your
    # code incorrectly (i.e. reading the 2nd file before validating the data from
    # the 1st file). You should ensure that your code fails at the right point-
    # always finish reading and checking one file before you open the next.

    # most of these tests should be easy to understand but clarification comments have been
    # added for some. You can also examine the data in the tests/data subfolders

    # Spoofing no args using empty strings
    echo "Usage"
    run_test ./$testExecutable "" "" 0 "Usage: $testExecutable file1 file2"
    
    # Note that although these files do not exist, the thing
    # which should cause the failure is that there are 3 arguments
    # NOT that the filenames are incorrect.
    echo ""
    echo "Bad Args"
    run_test ./$testExecutable "1 2" "3" 1 "ERROR: Bad Arguments"
    
    # Neither '1' nor '2' exists, but it should fail on '1' because
    # you should be opening, validating, reading and closing
    # before you access file 2.
    echo ""
    echo "Bad Filename"
    run_test ./$testExecutable "1" "2" 2 "ERROR: Bad File Name (1)"
    
    # we have deliberately removed read permissions 
    # for this file using chmod
    echo ""
    echo "Bad Permissions"
    filename="bad_perms"
    full_path=$path$filename$file_ext
    run_test ./$testExecutable $full_path "2" 2 "ERROR: Bad File Name ($full_path)"
    
    echo ""
    echo "Bad Magic Number"
    filename="bad_mn"
    full_path=$path$filename$file_ext
    run_test ./$testExecutable $full_path "2" 3 "ERROR: Bad Magic Number ($full_path)"

    echo ""
    echo "Bad Dimensions (big)"
    filename="bad_dims_high"
    full_path=$path$filename$file_ext
    run_test ./$testExecutable $full_path "tmp" 4 "ERROR: Bad Dimensions ($full_path)"

    echo ""
    echo "Bad Dimensions (small)"
    filename="bad_dims_low"
    full_path=$path$filename$file_ext
    run_test ./$testExecutable $full_path "tmp" 4 "ERROR: Bad Dimensions ($full_path)"

    # In this test, the dimensions used are within the permitted range for the file format
    # but are beyond the amount of memory which C can allocate.
    echo ""
    echo "Bad Malloc (dims too high to allocate)"
    filename="bad_malloc"
    full_path=$path$filename$file_ext
    run_test ./$testExecutable $full_path "tmp" 5  "ERROR: Image Malloc Failed"

    # data has a greyvalue above the maximum permitted value
    echo ""
    echo "Bad Data (too high)"
    filename="bad_data_high"
    full_path=$path$filename$file_ext
    run_test ./$testExecutable $full_path "tmp" 6 "ERROR: Bad Data ($full_path)"

    # data has a greyvalue below the minimum permitted value
    echo ""
    echo "Bad Data (too low)"
    filename="bad_data_low"
    full_path=$path$filename$file_ext
    run_test ./$testExecutable $full_path "tmp" 6 "ERROR: Bad Data ($full_path)"

    # too many greyvalues compared to the actual dimensions of the file
    echo ""
    echo "Bad Data (too much)"
    filename="bad_data_much"
    full_path=$path$filename$file_ext
    run_test ./$testExecutable $full_path "tmp" 6 "ERROR: Bad Data ($full_path)"

    # too few greyvalues compared to the actual dimensions of the file
    echo ""
    echo "Bad Data (too few)"
    filename="bad_data_little"
    full_path=$path$filename$file_ext
    run_test ./$testExecutable $full_path "tmp" 6 "ERROR: Bad Data ($full_path)"

    # check indexes 3:7 for the string 'Comp'
    # because Comp executables do not write out to a new file
    # we should not run this test for those executables.
    if [[ ${testExecutable:3:4} != "Comp" ]]
    then

    	# Because your error checking may detect being unable to open a file in
    	# write mode as either a bad output or a bad filename, and both are 
    	# acceptable methods, we will run two different tests, only one of which should pass,
    	# and manually ammend the total variable.
    	echo ""
    	echo "Bad Output (no write permissions)"
    	filename="good"
    	full_path=$path$filename$file_ext
    	echo
    	run_test ./$testExecutable $full_path "tests/data/bad.out" 8 "ERROR: Bad Output(tests/data/bad.out)"

    	echo "ALTERNATIVE - Bad File Name"
    	run_test ./$testExecutable $full_path "tests/data/bad.out" 2 "ERROR: Bad File Name (tests/data/bad.out)"
    	# Because this is functionally only one test, not 2
    	# we will subtract 2 from the total test counter.
    	total=$(( $total-2 ))
    fi

    ### Functionality Tests ###

    # to test the functionality, we need to check which 'class' of executable
    # we are working with - echo, comp or conversion. We will do this by looking
    # at substrings from the executable name.

    # note that where diff is used you may see slight differences if you have linebreaks
    # or spaces used differently. Linebreaks at the end of the file are bad practice BUT
    # will be accounted for in the final test.

    # if we have an Echo function:
    if [[ ${testExecutable:3:4} == "Echo" ]]
    then
        # To test echo we will:
        #    - check for the right code and message
        #    - and check that the files match using the diff tool
        echo ""
        echo "Testing $testExecutable Functionality"
        filename="good"
        full_path=$path$filename$file_ext
        run_test ./$testExecutable $full_path "tmp" 0 "ECHOED"
        D=$(diff $full_path tmp)
        if [[ $D != "" ]]
        then
            echo "FILES ARE DIFFERENT"
            echo $D
        else
            echo "FILES ARE IDENTICAL"
        fi
    # If it is a Comp executable...
    elif [[ ${testExecutable:3:4} == "Comp" ]]
    then
        # To test comp we will:
        #    - Test two identical files
        #    - Test two different files
        echo ""
        echo "Testing $testExecutable Functionality - identical"
        filename="good"
        comp_file="good2"
        diff_file="good3"
        full_path=$path$filename$file_ext
        echo "***************"$testExecutable $full_path $path$comp_file$file_ext 0 "IDENTICAL"
        run_test ./$testExecutable $full_path $path$comp_file$file_ext 0 "IDENTICAL"
        echo "Testing $testExecutable Functionality - different"
        echo "***************"$testExecutable $full_path $path$diff_file$file_ext 0 "DIFFERENT"
        run_test ./$testExecutable $full_path $path$diff_file$file_ext 0 "DIFFERENT"
    # if it is a conversion executable...
    elif [[ ${testExecutable:3:1} == "2" ]]
    then
        # To test conversion we will:
        #    - test for message & code
        #    - compare the converted file to an identical file in the other format
        echo ""
        echo "Testing $testExecutable Functionality"
        full_path=$path"good"$file_ext
        # because we are converting to another file type
        # we need the extension (which is the 3 letters starting from index 4.)
        to_ext=${testExecutable:4:3}
        # concatenating for tidyness as it is long
        comp_path="tests/data/"$to_ext"_data/good."$to_ext

        run_test ./$testExecutable $full_path "tmp" 0 "CONVERTED"
        D=$(diff $comp_path tmp) 
        if [[ $D != "" ]]
        then
            echo "CONVERTED FILES ARE DIFFERENT"
            echo $D
        else
            echo "CONVERTED FILES ARE IDENTICAL"
        fi
    fi
    
done

###### DO NOT REMOVE - restoring permissions
# git will be unable to deal with files when we don't have permissions
# so to prevent you having to deal with untracked files, we will restore
# permissions after running the tests.

# chmod +(w / r) means 'give all users write / read permission for the file'
chmod +w tests/data/bad.out
chmod +r tests/data/ebf_data/bad_perms.ebf
chmod +r tests/data/ebu_data/bad_perms.ebu
chmod +r tests/data/ebc_data/bad_perms.ebc
###### YOU CAN EDIT BELOW THIS POINT

# the run_test function has been incrementing both a counter for how many tests have run
# and a counter for passes. We will display this (along with a message for clarity).
echo "------------------------------------------------------------------------------"
echo "You passed $score out of $total"
echo "IMPORTANT: this is a count of tests passed - it is not your final grade."
echo "------------------------------------------------------------------------------"


# OPTIONAL - neither of these will affect your tests
# Many programmers prefer to work from a relatively 'tidy' directory
# and so will ensure any temp files created by the tests are removed
# and any object files are removed.

# remove the files we created during the tests
rm tmp null
# and run make clean to remove object files
make clean
