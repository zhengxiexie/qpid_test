#!/bin/bash

qpid-config add exchange topic news-service -b localhost:10001
qpid-config add exchange topic news-service -b localhost:10002
qpid-config add exchange topic news-service -b localhost:10003
qpid-config add exchange topic news-service -b localhost:10004
qpid-config add exchange topic news-service -b localhost:10005

qpid-route route add localhost:10002 localhost:10001 news-service topic1
qpid-route route add localhost:10002 localhost:10001 news-service topic2
qpid-route route add localhost:10003 localhost:10001 news-service topic3
qpid-route route add localhost:10004 localhost:10001 news-service topic4
qpid-route route add localhost:10005 localhost:10001 news-service topic5

