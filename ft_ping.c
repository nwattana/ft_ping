#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <time.h>

#define PACKET_SIZE 62
#define PING_TIMEOUT 1 // second

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void ping(const char *ip_addr) {
	int sockfd;
	struct sockaddr_in addr;
	struct icmp icmp_hdr;
	char packet[PACKET_SIZE];
	char buffer[PACKET_SIZE];

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) { 
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(addr));

	icmp_hdr.icmp_type = ICMP_ECHO;
	icmp_hdr.icmp_code = 0;
	icmp_hdr.icmp_id = getpid() & 0xFFF;
	icmp_hdr.icmp_seq = 1;
	icmp_hdr.icmp_cksum = 0;


	memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));
	icmp_hdr.icmp_cksum = checksum(packet, sizeof(icmp_hdr));
	memcpy(packet, &icmp_hdr, sizeof(icmp_hdr));

	struct timespec start, end;

	if (sendto(sockfd, packet, sizeof(icmp_hdr), 0, (struct sockaddr *)&addr, sizeof(addr) <= 0)) {
		perror("No reply received");
		exit(EXIT_FAILURE);
	}

	clock_gettime(CLOCK_MONOTONIC, &start);

	struct sockaddr_in reply_addr;
	socklen_t reply_len = sizeof(reply_addr);

	alarm(PING_TIMEOUT);
	if (recvfrom(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *)&reply_addr, &reply_len) <= 0) {
        perror("No reply received");
        exit(EXIT_FAILURE);
    }
	clock_gettime(CLOCK_MONOTONIC, &end);
	double rtt = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
	printf("Reply from %s: RTT = %.2f ms\n", ip_addr, rtt);
	close(sockfd);
}



int main(int ac, char **av)
{
	if (ac != 2)
	{
		fprintf(stderr, "Usage: %s <IP Address>\n", av[0]);
	}
	printf("%s", av[1]);

	const char *ip_addr = av[1];
	ping(ip_addr);
	return (0);
}