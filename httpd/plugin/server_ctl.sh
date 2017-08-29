#!/bin/bash

ROOT=$(pwd)
LIB=$ROOT/lib/lib
bin=httpd
port=80
ip=0
pid=""

if [ -z $LD_LIBRARY_PATH ];then
	export LD_LIBRARY_PATH=$LIB
fi

proc=$(basename $0)
function usage()
{
	printf "Usage:\n\t%s [start(-s)|stop(-t)|restart(-rt)|status(-st)]\n\n" "$proc"
}

is_exist(){
	pid=$(pidof $bin)
	if [ -z "$pid" ]; then
		return 1
	else
		return 0
	fi
}

function start_server(){
	if is_exist; then
		echo "server is running..., pid: $pid"
	else
		./$bin $ip $port
		echo "server start ... done"
	fi
}

function stop_server(){
	if is_exist; then
		kill -9 $pid
		echo "server stop ... done"
	else
		echo "server is not running!"
	fi
}

function restart_server(){
	stop_server
	start_server
}

function status_server(){
	if is_exist; then
		echo "server is exist: $pid"
	else
		echo "server is not exist"
	fi
}

if [ $# -ne 1 ];then
	usage
	exit 1
fi

case $1 in
	start | -s )
		start_server
	;;
	stop | -t )
		stop_server
	;;
	restart | -rt )
		restart_server
	;;
	status | -st )
		status_server
	;;
	* )
		usage
		exit 2
	;;
esac











