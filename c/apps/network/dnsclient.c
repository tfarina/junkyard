#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

struct dnsheader {
  uint16_t id;
  uint16_t flags;
  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;
};

struct dnsquestion {
  char *qname;
  size_t qnamelen;
  uint16_t qtype;
  uint16_t qclass;
};

#define FLAG_QR 0x8000U
#define FLAG_RD 0x0100U /* Recursion Desired - query flag */
#define FLAG_RA 0x0080U

/* Resource Record definitions. */

/* A host address type. */
#define RR_TYPE_A 1

/* The Internet class. */
#define RR_CLASS_IN 1

#define MAX_PACKETLEN 512
#define MAX_DOMAINLEN 255
#define DNS_PORT 53

static void write_uint16(void *dst, uint16_t data) {
  *(uint16_t*)dst = htons(data);
}

static uint16_t read_uint16(void *src) {
  return ntohs(*(uint16_t *)src);
}

int main(int argc, char **argv) {
  char dname[MAX_DOMAINLEN];
  struct dnsheader* header;
  struct dnsquestion* question;
  uint8_t *query_pkt;
  struct sockaddr_in to;
  socklen_t tolen = sizeof(to);
  int sockfd;
  uint8_t answer_pkt[MAX_PACKETLEN];
  struct sockaddr_storage from;
  socklen_t fromlen = sizeof(from);
  ssize_t rlen;

  if (argc != 2) {
    fprintf(stderr, "usage: %s domain\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  strcpy(dname, argv[1]);

  if (strlen(dname) > MAX_DOMAINLEN) {
    exit(EXIT_FAILURE);
  }

  header = malloc(sizeof(*header));
  memset(header, 0, sizeof(*header));

  header->id = rand();
  header->flags = FLAG_RD; //htons(1 << 8) htons(0x0100);
  header->qdcount = 1;
  header->ancount = 0;
  header->nscount = 0;
  header->arcount = 0;

  question = malloc(sizeof(*question) + strlen(dname) + 2);
  memset(question, 0, sizeof(*question));

  question->qnamelen = strlen(dname) + 2;
  question->qname = malloc(question->qnamelen);

  // dns_domain_fromdot
  char *label = NULL;
  char *dnamedup = strdup(dname);
  int len, total = 0;
  while ((label = strsep(&dnamedup, ".")) != NULL) {
    len = strlen(label);
    if (len > MAX_DOMAINLEN) {
      fprintf(stderr, "Label is too long");
      free(dnamedup);
      return -1;
    }
    question->qname[total++] = len;
    strcat(&question->qname[total], label);
    total += len;
  }
  free(dnamedup);

  question->qtype = htons(RR_TYPE_A);
  question->qclass = htons(RR_CLASS_IN);

  // Prepare the packet (header + question) with the query that will be sent to
  // the DNS server.
  size_t questionsize =
      question->qnamelen + sizeof(question->qtype) + sizeof(question->qclass);
  size_t query_pktlen = sizeof(*header) + questionsize;

  query_pkt = malloc(query_pktlen);

  int offset = 0;

  // HEADER
  write_uint16(query_pkt, header->id);
  write_uint16(query_pkt + 2, header->flags);
  write_uint16(query_pkt + 4, header->qdcount);
  write_uint16(query_pkt + 6, header->ancount);
  write_uint16(query_pkt + 8, header->nscount);
  write_uint16(query_pkt + 10, header->arcount);

  offset += sizeof(*header);

  // QUESTION: QNAME + QTYPE + QCLASS

  // QNAME
  memcpy(query_pkt + offset, question->qname, question->qnamelen);
  offset += question->qnamelen;

  // QTYPE
  memcpy(query_pkt + offset, &question->qtype, sizeof(question->qtype));
  offset += sizeof(question->qtype);

  // QCLASS
  memcpy(query_pkt + offset, &question->qclass, sizeof(question->qclass));

  // Prepare the UDP socket that will be used to send the query to the DNS
  // server.
  memset(&to, 0, tolen);
  to.sin_family = AF_INET;
  to.sin_port = htons(DNS_PORT);
  inet_pton(AF_INET, "8.8.8.8", &to.sin_addr);

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    fprintf(stderr, "socket creation failed: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Send the query.
  if (sendto(sockfd, query_pkt, query_pktlen, 0, (struct sockaddr *)&to,
             tolen) == -1) {
    fprintf(stderr, "sendto failed\n");
    exit(EXIT_FAILURE);
  }

  // Receive the response.
  if ((rlen = recvfrom(sockfd, answer_pkt, sizeof(answer_pkt), 0,
                       (struct sockaddr *)&from, &fromlen)) == -1) {
    fprintf(stderr, "recvfrom failed\n");
    exit(EXIT_FAILURE);
  }

  // Parse the response.
  struct dnsheader *response_header;
  response_header = malloc(sizeof(struct dnsheader));

  response_header->id = read_uint16(answer_pkt);
  response_header->flags = read_uint16(answer_pkt + 2);
  response_header->qdcount = read_uint16(answer_pkt + 4);
  response_header->ancount = read_uint16(answer_pkt + 6);
  response_header->nscount = read_uint16(answer_pkt + 8);
  response_header->arcount = read_uint16(answer_pkt + 10);

  printf(";; ->>HEADER<<-\n");
  printf(";; flags:");
  if ((response_header->flags & FLAG_QR) != 0) {
    printf(" qr");
  }
  if ((response_header->flags & FLAG_RD) != 0) {
    printf(" rd");
  }
  if ((response_header->flags & FLAG_RA) != 0) {
    printf(" ra");
  }

  printf("; ");
  printf("QUERY: %u, ", response_header->qdcount);
  printf("ANSWER: %u, ", response_header->ancount);
  printf("AUTHORITY: %u, ", response_header->nscount);
  printf("ADDITIONAL: %u\n", response_header->arcount);

  printf("\n");

  printf(";; MSG SIZE rcvd: %zd\n", rlen);

  return 0;
}
