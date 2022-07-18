#!/bin/bash

rm testResults.txt

for file in ./*.cvm
do
  echo "Testing file $file"
  echo "Testing file $file" >> testResults.txt
  ../VM esegui "$file" >> testResults.txt
  echo "" >> testResults.txt
  echo "" >> testResults.txt
done
