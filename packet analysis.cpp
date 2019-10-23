#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#include <pcap.h>
using namespace std;

void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet);

int tcpcount = 0;
int icmpcount = 0;
int udpcount = 0;
int ethercount = 0;
int ipcount = 0;
int arpcount = 0;

int main(int argc, char **argv)
{
    pcap_t *fp;
    char errbuf[PCAP_ERRBUF_SIZE];

    fp = pcap_open_offline("/home/kushal/capture.pcap", errbuf);

    if (pcap_loop(fp, 0, packetHandler, NULL) < 0)
    {

        cout << "pcap_loop() failed: " << pcap_geterr(fp);
        return 1;
    }
    cout << "---Protocol Summary------\n";
    cout << "ARP: " << arpcount << "\n";
    cout << "Ethernet: " << ethercount << "\n";
    cout << "IP Packets: " << ipcount << "\n";
    cout << "TCP Packets: " << tcpcount << "\n";
    cout << "UDP Packets: " << udpcount << "\n";
    cout << "ICMP Packets: " << icmpcount << "\n";

    pcap_close(fp);
    return 0;
}

void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    const struct ether_header *ethernetHeader;
    const struct ip *ipHeader;
    ethercount += 1;

    ethernetHeader = (struct ether_header *)packet;

    if (ntohs(ethernetHeader->ether_type) == ETHERTYPE_IP)
    {
        ipHeader = (struct ip *)(packet + sizeof(struct ether_header));
        if (ipHeader->ip_p == IPPROTO_TCP)
        {
            tcpcount += 1;
        }
        else if (ipHeader->ip_p == IPPROTO_ICMP)
        {
            icmpcount += 1;
        }
        else if (ipHeader->ip_p == IPPROTO_UDP)
        {
            udpcount += 1;
        }
        else if (ipHeader->ip_p == IPPROTO_IP)
        {
            ipcount += 1;
        }
    }
    if (ntohs(ethernetHeader->ether_type) == ETHERTYPE_ARP)
    {
        arpcount += 1;
    }
}
