###############################################################
############# Build the simulated network setting #############
###############################################################
sudo ip netns a ns1
sudo ip netns a ns2
sudo ip netns a ns-router

sudo ip l a veth1 type veth peer name veth1-router
sudo ip l a veth2 type veth peer name veth2-router

sudo ip l s veth1 netns ns1
sudo ip l s veth1-router netns ns-router

sudo ip l s veth2 netns ns2
sudo ip l s veth2-router netns ns-router

sudo ip netns exec ns1 ip a a 10.1.2.2/24 dev veth1
sudo ip netns exec ns1 ip l s veth1 up

sudo ip netns exec ns2 ip a a 10.1.3.3/24 dev veth2
sudo ip netns exec ns2 ip l s veth2 up

sudo ip netns exec ns-router ip a a 10.1.2.1/24 dev veth1-router
sudo ip netns exec ns-router ip a a 10.1.3.1/24 dev veth2-router
sudo ip netns exec ns-router ip l s veth1-router up
sudo ip netns exec ns-router ip l s veth2-router up

ip netns exec ns1 ip route add 10.1.3.0/24 via 10.1.2.1
ip netns exec ns2 ip route add 10.1.2.0/24 via 10.1.3.1
