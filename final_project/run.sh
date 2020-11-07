#!/bin/bash
# Program:
#       This program shows "Hello World!" in your screen.
# History:
# 2015/07/16	VBird	First release
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH
echo -e "$1"
echo -e "$2"
echo -e "$3"
echo -e $(eval "./project $1 $2 $3")
echo -e "Program Done"
echo -e $(eval "./verify $1 $2 $3")
exit 0