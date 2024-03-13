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

int socket_flag = 0;//0无连接，1是有连接
int cin_flag = 0; //0是线程，1是main里的
int port_port = (rand() % (30) + 44890);//随机获取端口
int mode_port = -1;//-1是初始，1是ls，2是get,3是put
string getIP();
string file_name;
string ip_local = getIP();
string ip_for_ls = getIP();
SOCKET seversocket;
SOCKET socket_temp1;
sockaddr_in temp_addr;
hostent* get_host_info() {//获取客户端信息
	char name[50];
	memset(name, '\0', sizeof(name));
	gethostname(name, sizeof(name));
	hostent* host_info = gethostbyname(name);
	return host_info;
}
string getIP() //获取本机IP地址
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
int find_port(char* recv_buff, USHORT* port, short port1, short port2) //确定端口号
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
	cout << "输入用户名" << endl;
	cin >> send_temp;
	send_temp = "USER " + send_temp + "\r\n";
	send(s, send_temp.c_str(), strlen(send_temp.c_str()), 0);
}
void login_pass(SOCKET s) {
	string send_temp;
	cout << "输入密码" << endl;
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
		cout << "临时端口绑定失败" << endl;
		closesocket(socket_temp);
		WSACleanup();
		return;
	}
	int len = sizeof(temp_addr);
	if (listen(socket_temp, 5) != 0) {
		cout << "监听失败" << endl;
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
	cout << "输入文件名" << endl;
	cin >> file_name;
	send_temp = "RETR " + file_name + "\r\n";
	send(s, send_temp.c_str(), strlen(send_temp.c_str()), 0);
	SOCKET socket_temp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	temp_addr.sin_family = AF_INET;
	temp_addr.sin_port = htons(port_port++);
	temp_addr.sin_addr.S_un.S_addr = inet_addr(ip_local.c_str());
	iresult = bind(socket_temp, (const sockaddr*)&temp_addr, sizeof(temp_addr));
	if (iresult != 0) {
		cout << "临时端口绑定失败" << endl;
		closesocket(socket_temp);
		WSACleanup();
		return;
	}
	int len = sizeof(temp_addr);
	if (listen(socket_temp, 5) != 0) {
		cout << "监听失败" << endl;
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
	ofs << get_buf << endl;//为什么这里不关闭套接字还可以进行下一步
}
void put(SOCKET s) {
	string send_temp;
	char recv_buf[200];
	int iresult;
	cout << "输入文件名" << endl;
	cin >> file_name;
	send_temp = "STOR " + file_name + "\r\n";
	send(s, send_temp.c_str(), strlen(send_temp.c_str()), 0);
	SOCKET socket_temp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	temp_addr.sin_family = AF_INET;
	temp_addr.sin_port = htons(port_port++); //下次创建连接时不会端口被占用
	temp_addr.sin_addr.S_un.S_addr = inet_addr(ip_local.c_str());
	iresult = bind(socket_temp, (const sockaddr*)&temp_addr, sizeof(temp_addr));
	if (iresult != 0) {
		cout << "临时端口绑定失败" << endl;
		closesocket(socket_temp);
		WSACleanup();
		return;
	}
	int len = sizeof(temp_addr);
	if (listen(socket_temp, 5) != 0) {
		cout << "监听失败" << endl;
	}
	socket_temp1 = accept(socket_temp, (sockaddr*)&temp_addr, &len);
}
void put_send() {
	ifstream ifs;
	ofstream ofs;
	char put_buf[1024];
	memset(put_buf, '\0', sizeof(put_buf));
	file_name = "E:\\FTP_send_file\\" + file_name;  //通用  C:\\Users\\
	ifs.open(file_name, ios::app);
	ifs >> put_buf;
	send(socket_temp1, put_buf, strlen(put_buf), 0);
	closesocket(socket_temp1);//连接完后要即使关闭套接字
}
void help() {
	cout << "1、open 与FTP服务器连接 [输入格式 open 目标ip]" << endl;
	cout << "2、get 下载文件 [输入格式: get 文件名 ] " << endl;//
	cout << "3、put 上传文件 [输入格式：put 文件名]" << endl;//
	cout << "4、pwd 显示当前文件夹的绝对路径 " << endl;
	cout << "5、ls 显示远方当前目录的文件 " << endl;//
	cout << "0、quit 与FTP服务器断开连接" << endl;
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
				cout << "输入服务器IP地址" << endl;
				cin >> sever_ip;
				seversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
				if (seversocket == INVALID_SOCKET) {
					cout << "套接字创建失败" << endl;
					WSACleanup();
					return 0;
				}
				hostent* host_info = get_host_info();  //自动获取本机信息
				sockaddr_in hints;
				hints.sin_family = AF_INET;
				hints.sin_port = htons(4000);
				hints.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(*(struct in_addr*)host_info->h_addr_list[0]));
				//				hints.sin_addr.S_un.S_addr = inet_addr("192.168.254.1");
				iresult = bind(seversocket, (const sockaddr*)&hints, sizeof(hints));
				if (iresult != 0) {
					cout << "套接字绑定失败" << GetLastError() << endl;
					WSACleanup();
					return 0;
				}
				sockaddr_in sever_addr;//服务器地址，与其进行交流
				sever_addr.sin_addr.S_un.S_addr = inet_addr(sever_ip.c_str());
				sever_addr.sin_port = htons(21);
				sever_addr.sin_family = AF_INET;
				iresult = connect(seversocket, (const sockaddr*)&sever_addr, sizeof(sever_addr));
				if (iresult != 0) {
					cout << "连接服务器失败" << GetLastError() << endl;
					WSACleanup();
					return 0;
				}
				cout << "连接服务器成功" << endl;
				cin_flag = 1;
				socket_flag = 1;
			}
			if (send_buf == "ls" && socket_flag == 1) {
				string port_part1, port_part2;
				int port_temp = port_port;
				string ip = for_ls_ip(getIP())+',';                                                     //自动获取
				itoa(htons(port_temp) & 0xff, (char*)port_part1.c_str(), 10);
				itoa((htons(port_temp) >> 8) & 0xff, (char*)port_part2.c_str(), 10);
				send_buf = "PORT " + ip + port_part1.c_str() + "," + port_part2.c_str() + "\r\n";
				send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);
				mode_port = 1;
				cin_flag = 1;
			}
			if (send_buf == "ls" && socket_flag == 0) {
				cout << "未连接" << endl;
				cin_flag = 0;
			}
			if (send_buf == "pwd" && socket_flag == 1) {
				send_buf = "XPWD\r\n";
				send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);
				cin_flag = 1;
			}
			if (send_buf == "pwd" && socket_flag == 0) {
				cout << "未连接" << endl;
				cin_flag = 0;
			}
			if (send_buf == "help") {
				help();
				cin_flag = 0;
			}
			if (send_buf == "quit" && socket_flag == 1) {
				send_buf = "QUIT\r\n";
				send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);
				cout << "连接断开" << endl;
				closesocket(seversocket);
				WSACleanup();
				cin_flag = 0;
				socket_flag = 0;
			}
			if (send_buf == "quit" && socket_flag == 0) {
				cout << "未连接" << endl;
				cin_flag = 0;
			}
			if (send_buf == "pascv" && socket_flag == 1) {
				send_buf = "pasv\r\n";
				send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);
				cin_flag = 0;
			}
			if (send_buf == "pascv" && socket_flag == 0) {
				cout << "未连接" << endl;
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
				cout << "未连接" << endl;
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
				cout << "未连接" << endl;
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
		cout << "客户端初始化失败" << endl;
		return 0;
	}
	cout << "客户端初始化成功" << endl;

	DWORD id = 0;
	CreateThread(NULL, 0, send_thread, 0, 0, &id);
	while (1) {
		if (socket_flag == 1)
			break;
	}
	WSAEVENT event = WSACreateEvent();//创建事件
	int index = 0, ieventtotal = 0;
	//事件句柄和套接字句柄表
	WSAEVENT eventarry[WSA_MAXIMUM_WAIT_EVENTS];
	SOCKET sockarry[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEventSelect(seversocket, event, FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT);
	eventarry[ieventtotal] = event; //将事件保存在eventarry数组中
	sockarry[ieventtotal] = seversocket; //将套接字保存在sockarry中
	ieventtotal++;
	while (TRUE) {
		//在所有事件上等待，只要有一个事件对象变为已授信状态，则函数返回
		//对每个事件都调用WSAWaitForMultipleEvents()函数，以确定其状态
		//发生的世家对象的索引，一般是句柄数组中最前面的的那爱一个
		index = WSAWaitForMultipleEvents(ieventtotal, eventarry, FALSE, WSA_INFINITE, FALSE);
		//然后调用循环一次处理后面的事件对象
		index = index - WSA_WAIT_EVENT_0; //兼容性
		if (index == WSA_WAIT_FAILED || index == WSA_WAIT_TIMEOUT) {
			//continue;
		}
		else {
			WSANETWORKEVENTS netevent; //收到通知消息，WSA
			WSAEnumNetworkEvents(sockarry[index], eventarry[index], &netevent);
			if (netevent.lNetworkEvents & FD_READ) {
				if (netevent.iErrorCode[FD_READ_BIT] == 0) {
					memset(recv_buf, '\0', sizeof(recv_buf));
					iresult = recv(seversocket, recv_buf, sizeof(recv_buf), 0);
					cout << recv_buf;
					memcpy(code_s, recv_buf, 3);
					int code = atoi(code_s);
					switch (code) {
					case 220: //用户名
						login_user(sockarry[index]);
						break;
					case 331: //密码
						login_pass(sockarry[index]);
						break;
					case 530: //失败返回码
						login_user(sockarry[index]);
						break;
					case 230: //登陆成功返回码
						cin_flag = 0;
						break;
					case 257: //pwd返回码
						cin_flag = 0;
						break;
					case 150: //ls端口返回码
						if (mode_port == 1)
							ls_cout();
						else if (mode_port == 2)
							get_recv();
						else if (mode_port == 3)
							put_send();
						break;
					case 226: //ls,get,put成功返回码
						cin_flag = 0;
						break;
					case 221: //结束返回码
						cin_flag = 0;
						break;
					case 200: //port成功
						if (mode_port == 1)
							ls(sockarry[index]);
						else if (mode_port == 2)
							get(sockarry[index]);
						else if (mode_port == 3)
							put(sockarry[index]);
						break;
					case 425: //NLST失败
						cin_flag = 0;
						break;
					case 550:
						cin_flag = 0;
						break;
					}
				}
				else {
					cout << "接受数据失败" << endl;
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

