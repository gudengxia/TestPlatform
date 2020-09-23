###############################################################
############ Delete the simulated network namespce ############
###############################################################
sudo ip netns exec ns2 ip l del veth2
sudo ip netns exec ns1 ip l del veth1
sudo ip netns del ns1
sudo ip netns del ns2
sudo ip netns del ns-router
