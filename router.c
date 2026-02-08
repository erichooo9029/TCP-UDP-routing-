#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include<sys/time.h>//時間
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <string>
#include <cstring>

using namespace std;

#define MTU 1500
#define BUFF_LEN 10000  //buffer size
#define PACKET_SIZE 1518
#define CPUBUFFER_SIZE 20000
#define CLIENT_IP "127.0.0.1"
#define SERVER_IP "127.0.0.2"
#define ROUTER_IP "127.0.0.3"
#define SERVER_PORT 9000
#define ROUTER_PORT 9002
#define CLIENT_PORT 9003

#define SA struct sockaddr

typedef struct IPHeader{
    uint8_t version_ihl;
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment_offset;
    uint8_t time_to_live;
    uint8_t protocol;
    uint16_t header_checksum;
    uint32_t source_ip;
    uint32_t destination_ip;
    uint32_t options;
    
}IPHeader;

typedef struct UDPHeader
{
  uint32_t source_port:16,
           dest_port:16;
  uint32_t Segment_Length:16,
           Checksum:16;
}UDPHeader;

typedef struct MACHeader{
    uint8_t sour_mac[6];        // source
    uint8_t des_mac[6];         // destination
    uint16_t fram_typ;        // frame type
    uint32_t crc;             // crc
}MACHeader; //18bytes

typedef struct Packet
{
  struct IPHeader ipheader;
  struct UDPHeader udpheader;
  struct MACHeader macheader;
  char buffer[MTU - 40];
}Packet;

int count = 0;




void print_serv_addr(struct sockaddr_in serv_addr) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serv_addr.sin_addr), ip, INET_ADDRSTRLEN);
    printf("IP: %s\n", ip);
    printf("Port: %d\n", ntohs(serv_addr.sin_port));
}


int tcpsocket(){
    int fd;
    int count=0;

    //while(count<10){ 
    //printf("new loop");
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[PACKET_SIZE] = {0};
    char cpu_buffer[]={0};
    //printf("new loops");
    // Socket creation
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation error");
        //exit(EXIT_FAILURE);
    }
    
 
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(ROUTER_PORT);
    
    
 
    
    // port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // listen 
    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
 
    // Waiting for connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    

    
    // 建一個新socket給server
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Failed to create server socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address struct
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) { //轉二進制
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }
    int avg_queuing_delay = 0;
    while(count<10){
     printf("---router 從收到client送的到傳送至server時間分析----\n");
    struct  timeval    tv;
    struct  timezone   tz;
    //memset(buffer, 0, sizeof(buffer));
    read(new_socket, buffer, PACKET_SIZE);
   //int64_t timestamp2  =atoi(buffer) ;//系統處理時間1
   // printf("system time1:");
    //printf("%ld %s \n", timestamp2,"usec");
   
    //memset(buffer, 0, sizeof(buffer));
    //read(new_socket, buffer, PACKET_SIZE);
    //cout<<"-----server receive-----"<<endl; 
    //cout<<"payload : ";
    //read(new_socket, buffer, PACKET_SIZE);
    //memcpy(cpu_buffer, buffer, PACKET_SIZE);
    gettimeofday(&tv,NULL);
   int64_t timestamp3  = tv.tv_sec * 1000000 + tv.tv_usec;//systemtime2
    printf("system time2:");
    printf("%ld %s\n", timestamp3,"usec");

    usleep(2000*1000);//設置處理時間
    
    
    gettimeofday(&tv,NULL);
   int64_t timestamp4  = tv.tv_sec * 1000000 + tv.tv_usec;//送出時間systemtime3
    printf("systemtime3:");
    printf("%ld %s\n", timestamp4,"usec");
   
    printf("waiting time:");
    int waiting_time=timestamp3-atoi(buffer) ; //waiting time 
    printf("%d %s\n", waiting_time,"usec");
    printf("service time:");
    int service_time=timestamp4-timestamp3; //service time 
    printf("%d %s\n", service_time,"usec");
    printf("queuing delay:");
    int queuing_delay=waiting_time+service_time; //queuing delay 
    printf("%d %s\n", queuing_delay,"usec");
    avg_queuing_delay = avg_queuing_delay*0.7 +queuing_delay*0.3; //歷史平均
    printf("avg queuing delay:");
    printf("%d %s\n", avg_queuing_delay,"usec");
    // Send the payload to the server
    send(server_sock, buffer, sizeof(buffer), 0);
    printf("router send tcp packet to server\n");
    //printf("client end_to_end_delay timestamp: %s\n", buffer);
    cout<<"-----router receive-----"<<endl; 
    cout<<"payload : ";
    // ... rest of your code ...
    for(int i=64;i<68;i++){                   //i=62-66
    	//printf("buffer[0] in decimal: %d\n", (unsigned char)buffer[62]);
    	cout<<buffer[i];
    	//cout<<buffer[69];
    }
    
    cout<<endl<<count;
    cout<<endl<<endl;
    //printf("jump1");
    //for (int i = 0; i < sizeof(buffer); i++) {
	//    printf("BUFFER%d內容 %d\n",i ,(unsigned char)buffer[i]);
	//}
	
    //printf("jump");
    printf("----------------------\n");
    
    count++;
    
    }
   
    close(server_sock);
    close(new_socket);
    close(server_fd);
    return 0;
}


void rcv_UDPpacket(int fd){
	struct IPHeader *iphdr = (struct IPHeader *)malloc(sizeof(struct IPHeader));
	struct UDPHeader *udphdr = (struct UDPHeader *)malloc(sizeof(struct UDPHeader));
	struct MACHeader *machdr = (struct MACHeader *)malloc(sizeof(struct MACHeader));
	struct Packet *packet = (struct Packet *)malloc(sizeof(struct Packet));
	socklen_t len;

	struct sockaddr_in clent_addr;
	
	char buf[BUFF_LEN];
        int cnt = 0;
        int avg_queuing_delay1 = 0;
	while(cnt<10)
	{
		
		cnt+=1;
    		memset(buf, 0, BUFF_LEN);
    		len = sizeof(clent_addr);
    		count = recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, &len);
    		printf("---router 從收到server送的到傳送至client時間分析----\n");
    		struct  timeval    tv;
		struct  timezone   tz;
                //int timestamp16  = atoi(packet->buffer);//送出時間systemtime3
                //printf("UDPsystemtime1:");
                //printf("%d %s\n", timestamp16,"usec");
		gettimeofday(&tv,NULL);
		int64_t timestamp11  = tv.tv_sec * 1000000 + tv.tv_usec;//系統處理時間2
		printf("UDPsystem time2:");
		printf("%ld %s \n", timestamp11,"usec");
		//gettimeofday(&tv,NULL);
		//int32_t timestamp10  = tv.tv_sec * 1000000 + tv.tv_usec;//系統處理時間1
		
		//printf("UDPsystem time1:");
		//printf("%d %s \n", timestamp10,"usec");
    		if(count == -1)
    		{
    			printf("client recieve data fail!\n");
    			return;
    		}
    		else{
    			printf("router rcv UDP packet %d !\n", cnt);
    		}

    		

    		memcpy(packet, buf, sizeof(*packet));
    		*iphdr = packet->ipheader;
    		*udphdr = packet->udpheader;
    		*machdr = packet->macheader;
    		
    		//建立udp socket給client
		int client_sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (client_sock < 0) {
		    perror("Failed to create client socket");
		    return ;
		}

		// Set up the client address struct
		struct sockaddr_in client_addr;
		client_addr.sin_family = AF_INET;
		client_addr.sin_port = htons(CLIENT_PORT);
		if (inet_pton(AF_INET, CLIENT_IP, &client_addr.sin_addr) <= 0) { //ip轉2進位
		    perror("inet_pton failed");
		    return ;
		}

		
		
		usleep(3000*1000);//UDP service time
		
		gettimeofday(&tv,NULL);
		int64_t timestamp12  = tv.tv_sec * 1000000 + tv.tv_usec;//時間systemtime3
		printf("UDPsystemtime3:");
		printf("%ld %s\n", timestamp12,"usec");
		   
		
		printf("waiting time:");
		int waiting_time=timestamp11-atoi(packet->buffer); //waiting time 
		printf("%d %s\n", waiting_time,"usec");
		printf("service time:");
		int service_time=timestamp12-timestamp11; //service time 
		printf("%d %s\n", service_time,"usec");
		printf("UDPqueuing delay:");
		int queuing_delay1=waiting_time+service_time; //queuing delay 
		printf("%d %s\n", queuing_delay1,"usec");
		avg_queuing_delay1 = avg_queuing_delay1*0.7 +queuing_delay1*0.3; //歷史平均
		printf("UDPavg queuing delay:");
		printf("%d %s\n", avg_queuing_delay1,"usec");
		printf("---------------------------\n");
		printf("UDP end_to_end_delay timestamp: %s%s\n", packet->buffer,"usec");
		sendto(client_sock, packet, sizeof(*packet), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
		
		close(client_sock);
    		
    	}
    	
    	printf("end");
}

int udpsocket()
{
    int cli_sockfd;
	struct sockaddr_in cli_addr;
	cli_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(cli_sockfd < 0)
	{
		printf("Socket create fail!\n");
		return -1;
	}
	
	memset(&cli_addr, 0, sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_addr.sin_port = htons(ROUTER_PORT);
	
	
	if(bind(cli_sockfd, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) < 0)
	{
		printf("Socket bind fail!\n");
		return -1;
	}
	rcv_UDPpacket(cli_sockfd);
	
	close(cli_sockfd);
	
	return 0;
}

int main()
{
        std::thread tcp_thread(tcpsocket);
    	std::thread udp_thread(udpsocket);
	tcp_thread.join();
    	udp_thread.join();	
	
	return 0;
}
