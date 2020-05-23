//�����ǿͻ��ˣ���

#include "winsock2.h"
#include <WS2tcpip.h>
#include <iostream>
#include<Windows.h>
#include<time.h>
#include<thread>
#include <cassert>
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;

bool inputcheck(char * bufSend);

static double send_time;
static SOCKET			sClient;			//�ͻ����׽���
SOCKADDR_IN		servAddr;			//��������ַ

const int		COMMAND_LENGTH = 4;
bool			gottenkey = false;
void sendcheck()
{
	while (true)
	{
		Sleep(108000000);  //��ʱ���趨Ϊ5s����һ�Σ����ڲ��ԣ�108000000Ϊ30min
		//double current_time = clock();
		//double durationTime = ((double)(current_time - send_time)) / CLK_TCK;
		//if (durationTime > 20)     //���ʱ�����30����
		//{
			char	bufSend[64] = { "CHCK1234" };	//�������ݻ�����
			cout << "send check";
			sendto(sClient, bufSend, strlen(bufSend), 0, (sockaddr*)&servAddr, sizeof(servAddr));
			send_time = clock();
			
		//}
	}
}
int main(int argc, char* argv[])
{
	const int		BUF_SIZE = 64;
	WSADATA			wsd;				//WSADATA����
	char			bufSend[BUF_SIZE];	//�������ݻ�����
	char			bufRecv[BUF_SIZE];  //�������ݻ�����
	int				retVal;				//����ֵ

	char* closeSymbol = (char*)"00";//����ͨ�ŵı�־
									  // ���÷���˵�ַ
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", (void*)&servAddr.sin_addr.S_un.S_addr);
	servAddr.sin_port = htons((short)4999);
	int	nServAddlen = sizeof(servAddr);
	int pid = (int)_getpid();
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
		cout << "���ι����е�PIDֵΪ" <<pid <<endl;
		cout << "�����͵�������'0'�����ʾ������˴�UDPͨ��\n";
		cout << "Client UDP Socket init !" << endl;
	}
	//std::thread t(sendcheck);
	//std::cout << "sendcheck thread\n";
	//t.detach();     //���߳̿��Էŷ�����

	// ѭ���ȴ��������˷������� & �ӷ���˽�������
	while (true)
	{
		// ��ʼ��buf�ռ�
		ZeroMemory(bufSend, BUF_SIZE);
		// �����˷�������buf
		cout << "Data send to Server UDP Socket: ";
		char a[10];
		cin >> bufSend >> a;
		strcat_s(bufSend, a);
		cout << bufSend;
		if (!inputcheck(bufSend)) {
			ZeroMemory(bufSend, BUF_SIZE);
			// �����˷�������buf
			cout << "Data send to Server UDP Socket: ";
			char a[10];
			cin >> bufSend >> a;
			strcat_s(bufSend, a);
			cout << bufSend;
		}
		retVal = sendto(sClient, bufSend, strlen(bufSend), 0, (sockaddr*)&servAddr, nServAddlen);
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
		retVal = recvfrom(sClient, bufRecv, BUF_SIZE, 0, (sockaddr*)&servAddr, &nServAddlen);
		bufRecv[retVal] = '\0';
		char str[INET_ADDRSTRLEN];
		cout << "Server IP��" << inet_ntop(AF_INET, &servAddr.sin_addr, str, sizeof(str)) << endl;
		cout << "Data recv from Server UDP Socket: " << bufRecv << endl;

		/*�ɹ����ȷ��*/
		if (!strncmp(bufRecv, "TICK", COMMAND_LENGTH))    
		{
			cout << "GOT TICK\n";
			gottenkey = true;
			char* ticketid;
			ticketid = &bufRecv[COMMAND_LENGTH];

			thread t(sendcheck);
			cout << "sendcheck thread\n";
			t.detach();     //���߳̿��Էŷ�����,�����̲߳�������
		}
		//�������ܾ�����
		else if (!strncmp(bufRecv, "FAIL", COMMAND_LENGTH)) {
			cout << "GOT FAIL\n";

		}
		else if (!strncmp(bufRecv, "THNX", COMMAND_LENGTH)) {
			cout << "GOT THANK\n";
			gottenkey = false;

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

bool inputcheck(char * bufSend) {
	bool iscorrect = true;
	if (gottenkey == true && !strncmp(bufSend, "HELO", COMMAND_LENGTH)) {
		cout << "��ʱ���Ѿ�ӵ����key�������ظ�����" << endl;
		iscorrect = false;
	}
	else if (strncmp(bufSend, "HELO", COMMAND_LENGTH) && strncmp(bufSend, "LISC", COMMAND_LENGTH) && strncmp(bufSend, "GBYE", COMMAND_LENGTH)) {
		cout << "WRONG INPUT" << endl;
		iscorrect = false;
	}
	return iscorrect;
}
