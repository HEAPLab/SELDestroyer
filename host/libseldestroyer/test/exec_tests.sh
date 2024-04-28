#!/bin/bash

FILES=$(find . -executable -type f)

for i in $FILES; do
    if [[ "$i" != "./exec_tests.sh" ]] then
        FILE=$(basename $i)
        echo "Executing test "$FILE"..."
        ./$FILE
        if [[ $? == 0 ]] then
            echo "   Test OK"
        else
            echo "!!! TEST FAILED"
            exit
        fi
    fi
done
