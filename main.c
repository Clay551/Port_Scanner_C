#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define COLOR_GREEN  "\033[1;32m"
#define COLOR_RED    "\033[1;31m"
#define COLOR_BLUE   "\033[1;34m"
#define COLOR_RESET  "\033[0m"

typedef enum {
    PORT_FTP = 21,
    PORT_SSH = 22,
    PORT_TELNET = 23,
    PORT_SMTP = 25,
    PORT_DNS = 53,
    PORT_TFTP = 69,
    PORT_HTTP = 80,
    PORT_POP3 = 110,
    PORT_NTP = 123,
    PORT_IMAP = 143,
    PORT_SNMP = 161,
    PORT_LDAP = 389,
    PORT_HTTPS = 443,
    PORT_SMB = 445,
    PORT_SYSLOG = 514,
    PORT_LDAPS = 636,
    PORT_MYSQL = 3306,
    PORT_RDP = 3389,
    PORT_POSTGRES = 5432,
    PORT_VNC = 5900,
    PORT_REDIS = 6379,
    PORT_ELASTIC = 9200,
    PORT_KAFKA = 9092,
    PORT_ZOOKEEPER = 2181,
    PORT_MONGODB = 27017
} CommonPorts;

typedef struct {
    int port;
    const char* name;
} PortInfo;

typedef struct {
    char ip[16];
    int port;
    const char* name;
} ThreadData;

PortInfo portList[] = {
    {21, "FTP"}, {22, "SSH"}, {23, "TELNET"}, {25, "SMTP"},
    {53, "DNS"}, {69, "TFTP"}, {80, "HTTP"}, {110, "POP3"},
    {123, "NTP"}, {143, "IMAP"}, {161, "SNMP"}, {389, "LDAP"},
    {443, "HTTPS"}, {445, "SMB"}, {514, "SYSLOG"}, {636, "LDAPS"},
    {3306, "MySQL"}, {3389, "RDP"}, {5432, "PostgreSQL"}, {5900, "VNC"},
    {6379, "Redis"}, {9200, "Elasticsearch"}, {9092, "Kafka"},
    {2181, "Zookeeper"}, {27017, "MongoDB"}
};
const int portCount = sizeof(portList) / sizeof(portList[0]);

void* scan_port(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        fprintf(stderr, COLOR_RED "[!] Socket error on port %d\n" COLOR_RESET, data->port);
        free(data);
        pthread_exit(NULL);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(data->port);
    addr.sin_addr.s_addr = inet_addr(data->ip);

    int result = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (result == 0) {
        printf(COLOR_GREEN "[+] Port open %d (%s)\n" COLOR_RESET, data->port, data->name);
    }

    close(sock);
    free(data);
    pthread_exit(NULL);
}

int main() {
    system("clear");
    char IP[16];
    memset(IP, '\0', sizeof(IP));

    printf(COLOR_BLUE "Enter IP: " COLOR_RESET);
    scanf("%15s", IP);

    pthread_t threads[portCount];

    for (int i = 0; i < portCount; ++i) {
        ThreadData* data = (ThreadData*)malloc(sizeof(ThreadData));
        strncpy(data->ip, IP, 15);
        data->port = portList[i].port;
        data->name = portList[i].name;

        if (pthread_create(&threads[i], NULL, scan_port, data) != 0) {
            fprintf(stderr, COLOR_RED "[!] Failed to create thread for port %d\n" COLOR_RESET, data->port);
            free(data);
        }
    }

    for (int i = 0; i < portCount; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

