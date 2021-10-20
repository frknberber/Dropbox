#CSE 344 FINAL TESTING SCRIPT
if [[ ! -f ./tekders.sh ]]; then 
	echo "Script must be run from its own dir." && exit
fi

svYExecPath="./ServerYExe"
svZExecPath="./ServerZExe"
cliExecPath="./ClientExe"

# Handle ctrl c
function ctrl_c() {
	echo -e "${RED}Script got SIGINT, quit...${NC}"
	pids=$(ps -ef | grep "$svYExecPath\|$svZExecPath|$cliExecPath" | tr -s " "| cut -d' ' -f 2 | xargs)
	printf "Killing %s\n" $pids
	if [[ $pids != "" ]]; then kill -SIGINT $pids; fi
	exit
	#read -e -p "Do you wish to quit?(y/n)" -n 1 ch
	#if [ "$ch" = "y" ]; then exit; fi
}
trap ctrl_c INT

#Test 1
read -e -p "Press any key to start test 1" -n 1
$svYExecPath 5545 127.0.0.1 5781 2 2 4 LOGY &
echo -e "${RED}Do not forget to tail server log file from another terminal.${NC}"
sleep 0.5
$svZExecPath 5781 2 3 LOGZ &
echo -e "${RED}Do not forget to tail server log file from another terminal.${NC}"
sleep 0.5
$cliExecPath testClient1 127.0.0.1 5545 &
sleep 0.5
$cliExecPath testClient2 127.0.0.1 5545 &
sleep 0.5
$cliExecPath testClient3 127.0.0.1 5545 &
sleep 6
echo "Test1 finished"

#Test 2
read -e -p "Press any key to start test 2" -n 1
$svYExecPath 5546 127.0.0.1 5782 2 2 3 LOGY2 &
echo -e "${RED}Do not forget to tail server log file from another terminal.${NC}"
sleep 0.5
$svZExecPath 5782 2 2 LOGZ2 &
echo -e "${RED}Do not forget to tail server log file from another terminal.${NC}"
sleep 0.5
$cliExecPath testClient1 127.0.0.1 5546 &
sleep 0.5
$cliExecPath testClient2 127.0.0.1 5546 &
sleep 0.5
$cliExecPath testClient3 127.0.0.1 5546 &
sleep 0.5
$cliExecPath testClient4 127.0.0.1 5546 &
sleep 0.5
$cliExecPath testClient5 127.0.0.1 5546 &
sleep 6
echo "Test2 finished"

#Test 3
read -e -p "Press any key to start test 3" -n 1
$svYExecPath 5547 127.0.0.1 5783 2 2 3 LOGY3 &
echo -e "${RED}Do not forget to tail server log file from another terminal.${NC}"
sleep 0.5
$svZExecPath 5783 1 2 LOGZ3 &
echo -e "${RED}Do not forget to tail server log file from another terminal.${NC}"
sleep 0.5
$cliExecPath testClient1 127.0.0.1 5547 &
sleep 0.5
$cliExecPath testClient2 127.0.0.1 5547 &
sleep 0.5
$cliExecPath testClient3 127.0.0.1 5547 &
sleep 0.5
$cliExecPath testClient4 127.0.0.1 5547 &
sleep 0.5
$cliExecPath testClient5 127.0.0.1 5547 &
sleep 6
echo "Test3 finished"

#Test 4
read -e -p "Press any key to start test 4" -n 1
$svYExecPath 5548 127.0.0.1 5784 2 1 3 LOGY4 &
echo -e "${RED}Do not forget to tail server log file from another terminal.${NC}"
sleep 0.5
$svZExecPath 5784 2 2 LOGZ4 &
echo -e "${RED}Do not forget to tail server log file from another terminal.${NC}"
sleep 0.5
$cliExecPath testClient1 127.0.0.1 5548 &
sleep 0.5
$cliExecPath testClient2 127.0.0.1 5548 &
sleep 0.5
$cliExecPath testClient3 127.0.0.1 5548 &
sleep 0.5
$cliExecPath testClient4 127.0.0.1 5548 &
sleep 0.5
$cliExecPath testClient5 127.0.0.1 5548 &
sleep 6
echo "Test4 finished"

ctrl_c