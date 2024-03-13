#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<map>
#include<string>
#include<fstream>
#include<cstdlib>
using namespace std;
#pragma const(lib,"ws2_32.lib")

int socket_flag = 0;//0�����ӣ�1��������
int cin_flag = 0; //0���̣߳�1��main���
int port_port = (rand() % (30) + 44890);//�����ȡ�˿�
int mode_port = -1;//-1�ǳ�ʼ��1��ls��2��get,3��put
string getIP();
string file_name;
string ip_local = getIP();
string ip_for_ls = getIP();
SOCKET seversocket;
SOCKET socket_temp1;
sockaddr_in temp_addr;
hostent* get_host_info() {//��ȡ�ͻ�����Ϣ
	char name[50];
	memset(name, '\0', sizeof(name));
	gethostname(name, sizeof(name));
	hostent* host_info = gethostbyname(name);
	return host_info;
}
string getIP() //��ȡ����IP��ַ
{
	WSADATA WSAData;
	char hostName[256];
	if (!WSAStartup(MAKEWORD(2, 0), &WSAData))
	{
		if (!gethostname(hostName, sizeof(hostName)))
		{
			hostent* host = gethostbyname(hostName);
			if (host != NULL)
			{
				return inet_ntoa(*(struct in_addr*)*host->h_addr_list);
			}
		}
	}
	return "Get IP failed.";
}
string for_ls_ip(string ip) {
	int i = strlen(ip.c_str());
	int j = 0;
	while (j < i) {
		if (ip[j] == '.')
			ip[j] = ',';
		j++;
	}
	return ip;
}
int find_port(char* recv_buff, USHORT* port, short port1, short port2) //ȷ���˿ں�
{
	int i = 0, j = 0;
	string port_temp;
	// 227 Entering Passive Mode (192,168,1,101,242,34)\r\n"
	// (192,168,186,1,4,0).
	while (recv_buff[i++] != '(');
	while (j < 4)
	{
		if (recv_buff[i++] == ',')
			j++;
	}

	while (recv_buff[i] != ',')
	{
		port_temp += recv_buff[i];
		i++;
	}
	port1 = atoi(port_temp.c_str());
	port1 = ntohs(port1);
	i++;
	while (recv_buff[i] != ')')
	{
		port_temp += recv_buff[i];
		i++;
	}
	port2 = atoi(port_temp.c_str());
	port2 = ntohs(port2);
	cout << port1 << " " << port2 << endl;
	return 0;
}
void login_user(SOCKET s) {
	string send_temp;
	int iresult;
	cout << "�����û���" << endl;
	cin >> send_temp;
	send_temp = "USER " + send_temp + "\r\n";
	send(s, send_temp.c_str(), strlen(send_temp.c_str()), 0);
}
void login_pass(SOCKET s) {
	string send_temp;
	cout << "��������" << endl;
	cin >> send_temp;
	send_temp = "PASS " + send_temp + "\r\n";
	send(s, send_temp.c_str(), strlen(send_temp.c_str()), 0);
}
int upload(SOCKET s) {
	return 0;
}
int download(SOCKET s) {
	return 0;
}
void ls(SOCKET s) {
	string send_temp;
	char recv_buf[200];
	int iresult;
	send_temp = "NLST\r\n";
	send(s, send_temp.c_str(), strlen(send_temp.c_str()), 0);
	SOCKET socket_temp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	temp_addr.sin_family = AF_INET;
	temp_addr.sin_port = htons(port_port++);
	temp_addr.sin_addr.S_un.S_addr = inet_addr(ip_local.c_str());
	iresult = bind(socket_temp, (const sockaddr*)&temp_addr, sizeof(temp_addr));
	if (iresult != 0) {
		cout << "��ʱ�˿ڰ�ʧ��" << endl;
		closesocket(socket_temp);
		WSACleanup();
		return;
	}
	int len = sizeof(temp_addr);
	if (listen(socket_temp, 5) != 0) {
		cout << "����ʧ��" << endl;
	}
	socket_temp1 = accept(socket_temp, (sockaddr*)&temp_addr, &len);
}
void ls_cout() {
	int iresult;
	char ls_buf[512];
	memset(ls_buf, '\0', sizeof(ls_buf));
	iresult = recv(socket_temp1, ls_buf, sizeof(ls_buf), 0);
	cout << ls_buf;
}
void get(SOCKET s) {
	string send_temp;
	char recv_buf[200];
	int iresult;
	cout << "�����ļ���" << endl;
	cin >> file_name;
	send_temp = "RETR " + file_name + "\r\n";
	send(s, send_temp.c_str(), strlen(send_temp.c_str()), 0);
	SOCKET socket_temp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	temp_addr.sin_family = AF_INET;
	temp_addr.sin_port = htons(port_port++);
	temp_addr.sin_addr.S_un.S_addr = inet_addr(ip_local.c_str());
	iresult = bind(socket_temp, (const sockaddr*)&temp_addr, sizeof(temp_addr));
	if (iresult != 0) {
		cout << "��ʱ�˿ڰ�ʧ��" << endl;
		closesocket(socket_temp);
		WSACleanup();
		return;
	}
	int len = sizeof(temp_addr);
	if (listen(socket_temp, 5) != 0) {
		cout << "����ʧ��" << endl;
	}
	socket_temp1 = accept(socket_temp, (sockaddr*)&temp_addr, &len);
}
void get_recv() {
	ifstream ifs;
	ofstream ofs;
	char get_buf[1024];
	memset(get_buf, '\0', sizeof(get_buf));
	recv(socket_temp1, get_buf, sizeof(get_buf), 0);
	file_name = "E:\\FTP_recv_file\\" + file_name;
	ofs.open(file_name, ios::app);
	ofs << get_buf << endl;//Ϊʲô���ﲻ�ر��׽��ֻ����Խ�����һ��
}
void put(SOCKET s) {
	string send_temp;
	char recv_buf[200];
	int iresult;
	cout << "�����ļ���" << endl;
	cin >> file_name;
	send_temp = "STOR " + file_name + "\r\n";
	send(s, send_temp.c_str(), strlen(send_temp.c_str()), 0);
	SOCKET socket_temp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	temp_addr.sin_family = AF_INET;
	temp_addr.sin_port = htons(port_port++); //�´δ�������ʱ����˿ڱ�ռ��
	temp_addr.sin_addr.S_un.S_addr = inet_addr(ip_local.c_str());
	iresult = bind(socket_temp, (const sockaddr*)&temp_addr, sizeof(temp_addr));
	if (iresult != 0) {
		cout << "��ʱ�˿ڰ�ʧ��" << endl;
		closesocket(socket_temp);
		WSACleanup();
		return;
	}
	int len = sizeof(temp_addr);
	if (listen(socket_temp, 5) != 0) {
		cout << "����ʧ��" << endl;
	}
	socket_temp1 = accept(socket_temp, (sockaddr*)&temp_addr, &len);
}
void put_send() {
	ifstream ifs;
	ofstream ofs;
	char put_buf[1024];
	memset(put_buf, '\0', sizeof(put_buf));
	file_name = "E:\\FTP_send_file\\" + file_name;  //ͨ��  C:\\Users\\
	ifs.open(file_name, ios::app);
	ifs >> put_buf;
	send(socket_temp1, put_buf, strlen(put_buf), 0);
	closesocket(socket_temp1);//�������Ҫ��ʹ�ر��׽���
}
void help() {
	cout << "1��open ��FTP���������� [�����ʽ open Ŀ��ip]" << endl;
	cout << "2��get �����ļ� [�����ʽ: get �ļ��� ] " << endl;//
	cout << "3��put �ϴ��ļ� [�����ʽ��put �ļ���]" << endl;//
	cout << "4��pwd ��ʾ��ǰ�ļ��еľ���·�� " << endl;
	cout << "5��ls ��ʾԶ����ǰĿ¼���ļ� " << endl;//
	cout << "0��quit ��FTP�������Ͽ�����" << endl;
}


DWORD WINAPI send_thread(LPVOID p) {
	string send_buf;
	int iresult;
	string sever_ip;
	while (1) {
		if (cin_flag == 0) {
			cout << "ftp> ";
			cin >> send_buf;
			if (send_buf == "open") {
				cout << "���������IP��ַ" << endl;
				cin >> sever_ip;
				seversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
				if (seversocket == INVALID_SOCKET) {
					cout << "�׽��ִ���ʧ��" << endl;
					WSACleanup();
					return 0;
				}
				hostent* host_info = get_host_info();  //�Զ���ȡ������Ϣ
				sockaddr_in hints;
				hints.sin_family = AF_INET;
				hints.sin_port = htons(4000);
				hints.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(*(struct in_addr*)host_info->h_addr_list[0]));
				//				hints.sin_addr.S_un.S_addr = inet_addr("192.168.254.1");
				iresult = bind(seversocket, (const sockaddr*)&hints, sizeof(hints));
				if (iresult != 0) {
					cout << "�׽��ְ�ʧ��" << GetLastError() << endl;
					WSACleanup();
					return 0;
				}
				sockaddr_in sever_addr;//��������ַ��������н���
				sever_addr.sin_addr.S_un.S_addr = inet_addr(sever_ip.c_str());
				sever_addr.sin_port = htons(21);
				sever_addr.sin_family = AF_INET;
				iresult = connect(seversocket, (const sockaddr*)&sever_addr, sizeof(sever_addr));
				if (iresult != 0) {
					cout << "���ӷ�����ʧ��" << GetLastError() << endl;
					WSACleanup();
					return 0;
				}
				cout << "���ӷ������ɹ�" << endl;
				cin_flag = 1;
				socket_flag = 1;
			}
			if (send_buf == "ls" && socket_flag == 1) {
				string port_part1, port_part2;
				int port_temp = port_port;
				string ip = for_ls_ip(getIP())+',';                                                     //�Զ���ȡ
				itoa(htons(port_temp) & 0xff, (char*)port_part1.c_str(), 10);
				itoa((htons(port_temp) >> 8) & 0xff, (char*)port_part2.c_str(), 10);
				send_buf = "PORT " + ip + port_part1.c_str() + "," + port_part2.c_str() + "\r\n";
				send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);
				mode_port = 1;
				cin_flag = 1;
			}
			if (send_buf == "ls" && socket_flag == 0) {
				cout << "δ����" << endl;
				cin_flag = 0;
			}
			if (send_buf == "pwd" && socket_flag == 1) {
				send_buf = "XPWD\r\n";
				send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);
				cin_flag = 1;
			}
			if (send_buf == "pwd" && socket_flag == 0) {
				cout << "δ����" << endl;
				cin_flag = 0;
			}
			if (send_buf == "help") {
				help();
				cin_flag = 0;
			}
			if (send_buf == "quit" && socket_flag == 1) {
				send_buf = "QUIT\r\n";
				send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);
				cout << "���ӶϿ�" << endl;
				closesocket(seversocket);
				WSACleanup();
				cin_flag = 0;
				socket_flag = 0;
			}
			if (send_buf == "quit" && socket_flag == 0) {
				cout << "δ����" << endl;
				cin_flag = 0;
			}
			if (send_buf == "pascv" && socket_flag == 1) {
				send_buf = "pasv\r\n";
				send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);
				cin_flag = 0;
			}
			if (send_buf == "pascv" && socket_flag == 0) {
				cout << "δ����" << endl;
				cin_flag = 0;
			}
			if (send_buf == "get" && socket_flag == 1) {
				mode_port = 2;
				string port_part1, port_part2;
				int port_temp = port_port;
				string ip = for_ls_ip(getIP()) + ',';
				itoa(htons(port_temp) & 0xff, (char*)port_part1.c_str(), 10);
				itoa((htons(port_temp) >> 8) & 0xff, (char*)port_part2.c_str(), 10);
				send_buf = "PORT " + ip + port_part1.c_str() + "," + port_part2.c_str() + "\r\n";
				send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);
				cin_flag = 1;
			}
			if (send_buf == "get" && socket_flag == 0) {
				cout << "δ����" << endl;
				cin_flag = 0;
			}
			if (send_buf == "put" && socket_flag == 1) {
				mode_port = 3;
				string port_part1, port_part2;
				int port_temp = port_port;
				string ip = for_ls_ip(getIP()) + ',';
				itoa(htons(port_temp) & 0xff, (char*)port_part1.c_str(), 10);
				itoa((htons(port_temp) >> 8) & 0xff, (char*)port_part2.c_str(), 10);
				send_buf = "PORT " + ip + port_part1.c_str() + "," + port_part2.c_str() + "\r\n";
				send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);
				cin_flag = 1;
			}
			if (send_buf == "put" && socket_flag == 0) {
				cout << "δ����" << endl;
				cin_flag = 0;
			}
		}
	}
}
int main() {
	WSADATA wsadata;
	int iresult, len, i = 0;
	char recv_buf[1000];
	char send_buf[512];
	char code_s[3];
	string send_temp;
	string send_temp_pass;
	string recv_temp;
	iresult = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (iresult != 0) {
		cout << "�ͻ��˳�ʼ��ʧ��" << endl;
		return 0;
	}
	cout << "�ͻ��˳�ʼ���ɹ�" << endl;

	DWORD id = 0;
	CreateThread(NULL, 0, send_thread, 0, 0, &id);
	while (1) {
		if (socket_flag == 1)
			break;
	}
	WSAEVENT event = WSACreateEvent();//�����¼�
	int index = 0, ieventtotal = 0;
	//�¼�������׽��־����
	WSAEVENT eventarry[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET sockarry[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEventSelect(seversocket, event, FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT);
	eventarry[ieventtotal] = event; //���¼�������eventarry������
	sockarry[ieventtotal] = seversocket; //���׽��ֱ�����sockarry��
	ieventtotal++;
	while (TRUE) {
		//�������¼��ϵȴ���ֻҪ��һ���¼������Ϊ������״̬����������
		//��ÿ���¼�������WSAWaitForMultipleEvents()��������ȷ����״̬
		//���������Ҷ����������һ���Ǿ����������ǰ��ĵ��ǰ�һ��
		index = WSAWaitForMultipleEvents(ieventtotal, eventarry, FALSE, WSA_INFINITE, FALSE);
		//Ȼ�����ѭ��һ�δ��������¼�����
		index = index - WSA_WAIT_EVENT_0; //������
		if (index == WSA_WAIT_FAILED || index == WSA_WAIT_TIMEOUT) {
			//continue;
		}
		else {
			WSANETWORKEVENTS netevent; //�յ�֪ͨ��Ϣ��WSA
			WSAEnumNetworkEvents(sockarry[index], eventarry[index], &netevent);
			if (netevent.lNetworkEvents & FD_READ) {
				if (netevent.iErrorCode[FD_READ_BIT] == 0) {
					memset(recv_buf, '\0', sizeof(recv_buf));
					iresult = recv(seversocket, recv_buf, sizeof(recv_buf), 0);
					cout << recv_buf;
					memcpy(code_s, recv_buf, 3);
					int code = atoi(code_s);
					switch (code) {
					case 220: //�û���
						login_user(sockarry[index]);
						break;
					case 331: //����
						login_pass(sockarry[index]);
						break;
					case 530: //ʧ�ܷ�����
						login_user(sockarry[index]);
						break;
					case 230: //��½�ɹ�������
						cin_flag = 0;
						break;
					case 257: //pwd������
						cin_flag = 0;
						break;
					case 150: //ls�˿ڷ�����
						if (mode_port == 1)
							ls_cout();
						else if (mode_port == 2)
							get_recv();
						else if (mode_port == 3)
							put_send();
						break;
					case 226: //ls,get,put�ɹ�������
						cin_flag = 0;
						break;
					case 221: //����������
						cin_flag = 0;
						break;
					case 200: //port�ɹ�
						if (mode_port == 1)
							ls(sockarry[index]);
						else if (mode_port == 2)
							get(sockarry[index]);
						else if (mode_port == 3)
							put(sockarry[index]);
						break;
					case 425: //NLSTʧ��
						cin_flag = 0;
						break;
					case 550:
						cin_flag = 0;
						break;
					}
				}
				else {
					cout << "��������ʧ��" << endl;
					closesocket(sockarry[index]);
				}
			}
			if (netevent.lNetworkEvents & FD_WRITE) {
				if (netevent.iErrorCode[FD_WRITE_BIT] == 0) {

				}
			}
			if (netevent.lNetworkEvents & FD_CLOSE) {
				index++;
			}
		}
	}
	return 0;

}

