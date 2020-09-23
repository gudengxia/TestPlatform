kex=akcn-hybrid
sig=p256_falcon512
FizzDir=/home/fzhang/MT/fizz-am/_build/bin
CertDir=../Certs
LogDir=Result
clt_log=Result/tm.txt
half_loss_rate=10.00 
#half_delay_time='50ms 5ms distribution normal' 
half_delay_time='2.5ms'
loss_rate_inc=`echo "scale=1;1/2" | bc`

if [ ! -d "${LogDir}" ];then
    mkdir "${LogDir}"
fi

echo "DEC\nkex:${kex}\nsig:${sig}\nhalf_delay_time:${half_delay_time}\n" >> ${LogDir}/tm.avg


sudo ip netns exec ns-router tc qdisc add dev veth1-router root netem delay ${half_delay_time} 
sudo ip netns exec ns-router tc qdisc add dev veth2-router root netem delay ${half_delay_time} 
sudo ip netns exec ns2 nohup ../Server/srv.exe 1>/dev/null 2>/dev/null &
sleep 1
srv_pid=`sudo ip netns exec ns2 pgrep srv.exe`
if [ -n "$srv_pid" ];then
    echo "Server Start"
else
    exit 0
fi

for i in `seq 0 20`;do
    echo "******************************"
    echo "half_loss_rate=$half_loss_rate"
    sudo ip netns exec ns-router tc qdisc change dev veth1-router root netem loss ${half_loss_rate}% delay ${half_delay_time}
    sudo ip netns exec ns-router tc qdisc change dev veth2-router root netem loss ${half_loss_rate}% delay ${half_delay_time}
    t_start=$(date "+%Y-%m-%d %H:%M:%S")
    echo "start time:$t_start"
    ###commad
    log_file=${LogDir}/${half_loss_rate}.log
    #sudo ip netns exec ns1 ../Client/clt.exe 1>${log_file}.log 2>${log_file}.err
    echo "half_loss_rate:${half_loss_rate}" >>${LogDir}/tm.avg
    sudo ip netns exec ns1 ../Client/clt.exe 1>>${LogDir}/tm.avg 2>/dev/null
    echo "\n" >>${LogDir}/tm.avg
	##command

    if [ -n ${clt_log} ];then
	    sudo cat ${clt_log} >> ${log_file}
	    sudo rm ${clt_log}
    fi

    t_end=$(date "+%Y-%m-%d %H:%M:%S")
    echo "end_time:$t_end"
    echo "******************************\n"
    half_loss_rate=`echo "$half_loss_rate-$loss_rate_inc" | bc | awk '{printf "%.2f", $0}'`
done
    
###############################################################
############# Delete the network setting ######################
###############################################################
sudo ip netns exec ns2 kill -9 ${srv_pid}
echo "Test Finished"
sudo ip netns exec ns-router tc qdisc del dev veth1-router root
sudo ip netns exec ns-router tc qdisc del dev veth2-router root
