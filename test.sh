#!/bin/bash

for FILE in "$@"
do
  ./application "${FILE}" >> /dev/null
  if [ $? -eq 0 ]
  then
    echo -e $FILE" \033[0;32mok!\033[0m";
  else
    echo -e $FILE" \033[0;91mfailed!\033[0m";
  fi
done
