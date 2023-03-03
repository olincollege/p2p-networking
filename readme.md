# P2P Networking

## Peer Exchange 
In this project, we implemented a simplified model of peer exchange, often refered to as PEX (https://en.wikipedia.org/wiki/Peer_exchange). The concept is frequently used in datastructures such as distributed hash tables (DHT) which are integral to modern bittorrent protocols that try to avoid a centralized point of failure that a tracker may provide. 

Peer exchange begins with a process known as "bootstrapping" where there are a few well known centralized nodes that clients intially first try to hit. As an example, the official libtorrent organization hosts a bootstrap node at dht.libtorrent.org:25401 (https://blog.libtorrent.org/2016/09/dht-bootstrap-node/). 

Below we provide an example of how boostrapping might be architected. 

<p align="center">
  <img src="./docs/bootstrap-1.png" width="500"/>
</p>
<p align="center">
  A client connects to a known bootstrap node. The node records the client's IP/port and tells the client that it does not know anything yet. 
</p>
