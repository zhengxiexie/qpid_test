#!/bin/bash

date_start=$(date +"%s")
	qpid-send -m 100000 -b 115.28.36.44:10001 -a hello-world --content-string $i --durable no
date_end=$(date +"%s")
diff=$(($date_end-$date_start))
echo -e "done\n"
echo -e "$diff seconds\n"
