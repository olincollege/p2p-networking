# P2P Networking

## Bootstrapping 
In this project, we implemented a simplified model of peer exchange, often refered to as PEX (https://en.wikipedia.org/wiki/Peer_exchange). The concept is frequently used in datastructures such as distributed hash tables (DHT) which are integral to modern bittorrent protocols that try to avoid a centralized point of failure that a tracker may provide. 

Peer exchange begins with a process known as "bootstrapping" where there are a few well known centralized nodes that clients intially first try to hit. As an example, the official libtorrent organization hosts a bootstrap node at dht.libtorrent.org:25401 (https://blog.libtorrent.org/2016/09/dht-bootstrap-node/). 

Below we provide an example of how boostrapping might be architected. 

<p align="center">
  <img src="./docs/bootstrap-1.png" width="500"/>
</p>
<p align="center">
  Client "A" connects to a known bootstrap node. The node records the client's IP/port and tells the client that it does not know anything yet. 
</p>


<p align="center">
  <img src="./docs/bootstrap-2.png" width="500"/>
</p>
<p align="center">
  Client "B" connects to a known bootstrap node. The node records the client's IP/port and returns the IP/port of client "A" that "B" can now   connect to. 
</p>

<p align="center">
  <img src="./docs/bootstrap-3.png" width="500"/>
</p>
<p align="center">
  Client "C" connects to a known bootstrap node. The node records the client's IP/port and returns the IP/port of client "A" and "B" that "C" can now connect to. 
</p>

## Peer Exchange 
After bootstrapping, clients engage in a process known as peer exchange where clients continuiously share the IP/port information of clients that they known about with other clients. You might realize that this is actually the same implementation as bootstrapping. In fact, all clients are actually completely valid bootstrapping servers themselves. 



