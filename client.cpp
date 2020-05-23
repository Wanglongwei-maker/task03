#include "winsock2.h"
#include <WS2tcpip.h>
#include <iostream>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;

char * pidTo6char(int pid);

int main(int argc, char* argv[])
{
	const int		BUF_SIZE = 64;
	const int		COMMAND_LENGTH = 4;
	WSADATA			wsd;				//WSADATA����
	SOCKET			sClient;			//�ͻ����׽���
	SOCKADDR_IN		servAddr;			//��������ַ
	char			bufSend[BUF_SIZE];	//�������ݻ�����
	char			bufRecv[BUF_SIZE];  //�������ݻ�����
	int				retVal;				//����ֵ
	char*			closeSymbol=(char *)"00";//����ͨ�ŵı�־
	
									  // ���÷���˵�ַ
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void*)&servAddr.sin_addr.S_un.S_addr);
	servAddr.sin_port = htons((short)4999);
	int	nServAddlen = sizeof(servAddr);
	int pid = (int)_getpid();
	char * strpid = pidTo6char(pid);
	// ��ʼ���׽��ֶ�̬��
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	// ����������׽���
	sClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == sClient)
	{
		cout << "Socket failed !" << endl;
		WSACleanup();               //�ͷ��׽�����Դ
		return  -1;
	}
	else
	{
		cout << "��ǰ����Ľ��̺���: " << pid << endl;
		cout << "�����͵�������'0'�����ʾ������˴�UDPͨ��\n";
		cout << "Client UDP Socket init !" << endl;
	}
	// ѭ���ȴ��������˷������� & �ӷ���˽�������
	while (true)
	{
		// ��ʼ��buf�ռ�
		ZeroMemory(bufSend, BUF_SIZE);
		// �����˷�������buf
		cout << "Data send to Server UDP Socket: ";	
		char a[10];
		cin >> bufSend >>a;
		strcat_s(bufSend,a);
		retVal = sendto(sClient, bufSend, strlen(bufSend), 0, (sockaddr *)&servAddr, nServAddlen);
		if (SOCKET_ERROR == retVal)
		{
			cout << "send failed!" << endl;
			closesocket(sClient);	//�رշ�����׽���			
			WSACleanup();		    //�ͷ��׽�����Դ
			return -1;
		}
		if (!strcmp(bufSend, closeSymbol))
		{
			cout << "Client UDP Socket wants to finish this communication" << endl;
			break;
		}

		// �ӷ���˽�������bufRecv
		retVal = recvfrom(sClient, bufRecv, BUF_SIZE, 0, (sockaddr *)&servAddr, &nServAddlen);
		bufRecv[retVal] = '\0';
		char str[INET_ADDRSTRLEN];
		cout << "Server IP��" << inet_ntop(AF_INET, &servAddr.sin_addr, str, sizeof(str)) << endl;
		cout << "Data recv from Server UDP Socket: " << bufRecv << endl;
		if (!strncmp(bufRecv, "TICK", COMMAND_LENGTH))
		{
			cout << "GOT TICK\n";
			char* ticketid;
			ticketid = &bufRecv[COMMAND_LENGTH];

		}
		else if (!strncmp(bufRecv, "FAIL", COMMAND_LENGTH)) {
			cout << "GOT FAIL\n";

		}
		else if (!strncmp(bufRecv, "THNK", COMMAND_LENGTH)) {
			cout << "GOT THNK\n";

		}
		// ������˷��͵�������'0'�����ʾ�����������˴�UDPͨ��	
		if (!strcmp(bufRecv, closeSymbol))
		{
			cout << "Server UDP Socket wants to finish this communication" << endl;
			break;
		}
		
	}
	//�˳�
	closesocket(sClient);	//�رշ�����׽���
	WSACleanup();
	//�ͷ��׽�����Դ
	Sleep(5000);
	return 0;
}

char * pidTo6char(int pid) {
	char s[10];
	char strpid5[10];
	ZeroMemory(s, 10);
	ZeroMemory(strpid5, 10);
	int i = 1;
	int temp = pid;
	while (temp /= 10) {
		i++;
	}
	for (int n = i; n <= 5; n++) {
		strcat_s(s, "0");
	}
	snprintf(strpid5, sizeof(strpid5), "%d", pid);
	strcat_s(s, strpid5);
	cout << s << endl;
	return s;
}
