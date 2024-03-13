# 一．概述

**基本要求**：根据FTP协议实现FTP客户端

**内容描述**：

(1)客户端应用字符界面形式，通信模型使用指定的模型。

(2)能实现FTP协议的部分功能,即实现FTP的登陆、ls、put、get、pwd命令

# 二．系统分析

（**系统解决方案**、功能模块化分）

## 1、找服务器

上网搜索市面流行的FTP服务器，并根据Windows的cmd命令窗口了解FTP客户端的实现命令窗口。从而确定输入界面。

2、了解模型的基本应用
   事件通知模型的read主要负责接受信息，如果将多个发送连接在一起，会导致相关功能无法正常运行，所以，使用线程专门用来发送功能请求。并设置cin_flag变量来限定此时是main在运行还是线程在使用。如果cin_flag为0，则此时是线程在运行，如果cin_flag为1，那么此时卖弄在运行。

2.1、参考Windows自带的cmd中ftp
   仿照其输入的格式以及输入的顺序规划登陆、ls、put、get、pwd以及help功能的实现规则。根据对ftp应用程序的使用，每个事件都会对应一个三位数的代码，所以要判断事件可以根据前三位的代码。

2.2、help模块的实现。
   参考Windows的cmd中可在未连接的情况下提示ftp客户端的命令提示，所以不加连接限定。把可实现功能的输入方式一并写入help模块中。

2.3、登陆模块
   参考cmd中ftp登陆，并且使用wiresharke抓包分析，为了方便分析，在虚拟机上使用FileZilla Server Interface建立一个FTP服务器，并在虚拟机上抓包分析。过滤条件 ip.addr==192.168.254.1(本机IP地址)。![img](D:\screen shout\typora\clip_image003.jpg)
 根据所抓数据包分析，客户端先于服务器建立连接，需要自己输入IP地址，且连接的端口是21号端口，如果连接成功，返回“220 Please visit https://filezilla-project.org/”，而后传输用户名，成功“331 Password required for test”,则再发送密码，，最后，满足以上条件，返回“230 Logged on”，登陆成功。再查看发送数据的格式。
 用户名 ：USER test\r\n
 密码  ：PASS 123456\r\n
 在客户端编写相关功能时，发送的数据符合规则即可。
 此外，在用户或密码输入错误的情况下，
 ![img](D:\screen shout\typora\clip_image005.jpg)
 ![img](D:\screen shout\typora\clip_image007.jpg)
 抓包可得，其会返回530，登陆失败报文“530 Login or password incorrect!”，此时需要重新输入密码，故客户端代码在530情况下返回输入账号环节。

2.4、pwd模块，获得绝对路径。抓包分析
 
 ![img](D:\screen shout\typora\clip_image009.jpg)
 由包可知，客户端向服务端发送请求，服务端回应
 ![img](D:\screen shout\typora\clip_image011.jpg)
 ![img](D:\screen shout\typora\clip_image013.jpg)
 客户端向服务器发送“XPWD\r\n”
            收到257 "/" is current directory.
 说明此时客户端可以访问到绝对路径。 

2.5、ls显示远方当前目录文件
 ![img](D:\screen shout\typora\clip_image015.jpg)
 抓包分析可得，客户端首先开放一个端口，并把它发给服务器，“PORT 192,168,254,1,29,150\r\n”，29*254+150就是客户端开放的端口号。服务器收到并返回200，随后客户端再向服务器发送“NLST\r\n”，随后服务器根据之前的端口向客户端发送连接，经过三次握手建立了临时连接，用以传输数据。最后，再传输成功之后，服务器发送“226 Successfully transferred "/"\r\n”。改换届他们会自动关闭连接，无需在客户端在客户端结束套接字。此外，该功能再无连接状态时无法使用的。定义变量socket_flag用以确认连接状态，如果socket_flag等于0则说明此时无连接，为1则说明已连接。

2.6、Get上传文件
 ![img](D:\screen shout\typora\clip_image017.jpg)
    通过抓包分析，客户端先向服务器开放一个端口，并将其发送给服务器。“PORT 192,168,254,1,29,150\r\n”，客户端在收到“200 Port command successful\r\n”之后，发送“RETR file1.txt\r\n”，表明自己要接受file1文件。随后，与ls一样，建立tcp临时连，在成功建立后服务器向客户端发送“150 Opening data channel for file download from server of "/file1.txt"\r\n”，随后通过ftp协议向客户端发送文件内的数据“12342394\r\n”，在传完后FTP服务器发送关闭连接请求，收到客户端的确认后FTP协议回应“226 Successfully transferred "/file1.txt"\r\n”，最后客户端回应，再发送关闭连接请求，服务器确认，改流程结束。

2.7、Put上传文件
 ![img](D:\screen shout\typora\clip_image019.jpg)
    通过抓包分析，流量图
 ![img](D:\screen shout\typora\clip_image021.jpg)
 根据此图分析流量的流向以及传输的内容，客户端现象服务端开放端口，并通过ftp port主动连接发送给服务器，服务器成功接受后回应200，客户端向虚拟机发送“STOR hello.txt\r\n”。随后，建立以客户端为服务器端的临时连接。建立连接成功后，ftp服务器回应“150 Opening data channel for file upload to server of "/hello.txt"\r\n”，传输数据，“hello”，数据传输完毕，关闭连接。随后，虚拟机向客户端发送226 Successfully transferred "/hello.txt"\r\n。
 

2.8、quit 与FTP服务器断开连接
     ![img](D:\screen shout\typora\clip_image023.jpg)
    客户端先向虚拟机发送“QUIT\r\n”，虚拟机收到后向客户端回应“221 Goodbye\r\n”，随后由客户端发送关闭连接请求，四次挥手后连接关闭，流程结束。

2.9、FTP服务器界面
 ![img](D:\screen shout\typora\clip_image025.jpg)

# 三．系统实现

## 1、线程（实现一级功能命令的初始发送）

DWORD WINAPI send_thread(LPVOID p) {

  string send_buf;

  int iresult;

  string sever_ip;

  while (1) {

​     if (cin_flag == 0) {

​       cout << "ftp> ";

​       cin >> send_buf;

​       if (send_buf == "open") {

​         cout << "输入服务器IP地址" << endl;

​         cin >> sever_ip;

​         seversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

​         if (seversocket == INVALID_SOCKET) {

​           cout << "套接字创建失败" << endl;

​           WSACleanup();

​           return 0;

​         }

​         hostent* host_info = get_host_info(); //自动获取本机信息

​         sockaddr_in hints;

​         hints.sin_family = AF_INET;

​         hints.sin_port = htons(4000);

​         hints.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(*(struct in_addr*)host_info->h_addr_list[0]));

​         //        hints.sin_addr.S_un.S_addr = inet_addr("192.168.254.1");

​         iresult = bind(seversocket, (const sockaddr*)&hints, sizeof(hints));

​         if (iresult != 0) {

​           cout << "套接字绑定失败" << GetLastError() << endl;

​           WSACleanup();

​           return 0;

​         }

​         sockaddr_in sever_addr;//服务器地址，与其进行交流

​         sever_addr.sin_addr.S_un.S_addr = inet_addr(sever_ip.c_str());

​         sever_addr.sin_port = htons(21);

​         sever_addr.sin_family = AF_INET;

​         iresult = connect(seversocket, (const sockaddr*)&sever_addr, sizeof(sever_addr));

​         if (iresult != 0) {

​           cout << "连接服务器失败" << GetLastError() << endl;

​           WSACleanup();

​           return 0;

​         }

​         cout << "连接服务器成功" << endl;

​         cin_flag = 1;

​         socket_flag = 1;

​       }

​       if (send_buf == "ls" && socket_flag == 1) {

​         string port_part1, port_part2;

​         int port_temp = port_port;

​         string ip = for_ls_ip(getIP())+',';                           //自动获取

​         itoa(htons(port_temp) & 0xff, (char*)port_part1.c_str(), 10);

​         itoa((htons(port_temp) >> 8) & 0xff, (char*)port_part2.c_str(), 10);

​         send_buf = "PORT " + ip + port_part1.c_str() + "," + port_part2.c_str() + "\r\n";

​         send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);

​         mode_port = 1;

​         cin_flag = 1;

​       }

​       if (send_buf == "ls" && socket_flag == 0) {

​         cout << "未连接" << endl;

​         cin_flag = 0;

​       }

​       if (send_buf == "pwd" && socket_flag == 1) {

​         send_buf = "XPWD\r\n";

​         send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);

​         cin_flag = 1;

​       }

​       if (send_buf == "pwd" && socket_flag == 0) {

​         cout << "未连接" << endl;

​         cin_flag = 0;

​       }

​       if (send_buf == "help") {

​         help();

​         cin_flag = 0;

​       }

​       if (send_buf == "quit" && socket_flag == 1) {

​         send_buf = "QUIT\r\n";

​         send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);

​         cout << "连接断开" << endl;

​         closesocket(seversocket);

​         WSACleanup();

​         cin_flag = 0;

​         socket_flag = 0;

​       }

​       if (send_buf == "quit" && socket_flag == 0) {

​         cout << "未连接" << endl;

​         cin_flag = 0;

​       }

​       if (send_buf == "pascv" && socket_flag == 1) {

​         send_buf = "pasv\r\n";

​         send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);

​         cin_flag = 0;

​       }

​       if (send_buf == "pascv" && socket_flag == 0) {

​         cout << "未连接" << endl;

​         cin_flag = 0;

​       }

​       if (send_buf == "get" && socket_flag == 1) {

​         mode_port = 2;

​         string port_part1, port_part2;

​         int port_temp = port_port;

​         string ip = for_ls_ip(getIP()) + ',';

​         itoa(htons(port_temp) & 0xff, (char*)port_part1.c_str(), 10);

​         itoa((htons(port_temp) >> 8) & 0xff, (char*)port_part2.c_str(), 10);

​         send_buf = "PORT " + ip + port_part1.c_str() + "," + port_part2.c_str() + "\r\n";

​         send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);

​         cin_flag = 1;

​       }

​       if (send_buf == "get" && socket_flag == 0) {

​         cout << "未连接" << endl;

​         cin_flag = 0;

​       }

​       if (send_buf == "put" && socket_flag == 1) {

​         mode_port = 3;

​         string port_part1, port_part2;

​         int port_temp = port_port;

​         string ip = for_ls_ip(getIP()) + ',';

​         itoa(htons(port_temp) & 0xff, (char*)port_part1.c_str(), 10);

​         itoa((htons(port_temp) >> 8) & 0xff, (char*)port_part2.c_str(), 10);

​         send_buf = "PORT " + ip + port_part1.c_str() + "," + port_part2.c_str() + "\r\n";

​         send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);

​         cin_flag = 1;

​       }

​       if (send_buf == "put" && socket_flag == 0) {

​         cout << "未连接" << endl;

​         cin_flag = 0;

​       }

​     }

  }

}

2、Help
 void help() {

  cout << "1、open 与FTP服务器连接 [输入格式 open 目标ip]" << endl;

  cout << "2、get 下载文件 [输入格式: get 文件名 ] " << endl;//

  cout << "3、put 上传文件 [输入格式：put 文件名]" << endl;//

  cout << "4、pwd 显示当前文件夹的绝对路径 " << endl;

  cout << "5、ls 显示远方当前目录的文件 " << endl;//

  cout << "0、quit 与FTP服务器断开连接" << endl;

}

![help](D:\screen shout\typora\clip_image026.png)

![img](D:\screen shout\typora\clip_image028.jpg)

 

## 3、open

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

 

if (send_buf == "open") {

​         cout << "输入服务器IP地址" << endl;

​         cin >> sever_ip;

​         seversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

​         if (seversocket == INVALID_SOCKET) {

​           cout << "套接字创建失败" << endl;

​           WSACleanup();

​           return 0;

​         }

​         hostent* host_info = get_host_info(); //自动获取本机信息

​         sockaddr_in hints;

​         hints.sin_family = AF_INET;

​         hints.sin_port = htons(4000);

​         hints.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(*(struct in_addr*)host_info->h_addr_list[0]));

​         //        hints.sin_addr.S_un.S_addr = inet_addr("192.168.254.1");

​         iresult = bind(seversocket, (const sockaddr*)&hints, sizeof(hints));

​         if (iresult != 0) {

​           cout << "套接字绑定失败" << GetLastError() << endl;

​           WSACleanup();

​           return 0;

​         }

​         sockaddr_in sever_addr;//服务器地址，与其进行交流

​         sever_addr.sin_addr.S_un.S_addr = inet_addr(sever_ip.c_str());

​         sever_addr.sin_port = htons(21);

​         sever_addr.sin_family = AF_INET;

​         iresult = connect(seversocket, (const sockaddr*)&sever_addr, sizeof(sever_addr));

​         if (iresult != 0) {

​           cout << "连接服务器失败" << GetLastError() << endl;

​           WSACleanup();

​           return 0;

​         }

​         cout << "连接服务器成功" << endl;

​         cin_flag = 1;

​         socket_flag = 1;

​       }

![img](D:\screen shout\typora\clip_image030.png)

![img](D:\screen shout\typora\clip_image032.jpg)

## 4、ls

if (send_buf == "ls" && socket_flag == 1) {

​         string port_part1, port_part2;

​         int port_temp = port_port;

​         string ip = for_ls_ip(getIP())+',';                           //自动获取

​         itoa(htons(port_temp) & 0xff, (char*)port_part1.c_str(), 10);

​         itoa((htons(port_temp) >> 8) & 0xff, (char*)port_part2.c_str(), 10);

​         send_buf = "PORT " + ip + port_part1.c_str() + "," + port_part2.c_str() + "\r\n";

​         send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);

​         mode_port = 1;

​         cin_flag = 1;

​       }

​       if (send_buf == "ls" && socket_flag == 0) {

​         cout << "未连接" << endl;

​         cin_flag = 0;

​       }

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

​     cout << "临时端口绑定失败" << endl;

​     closesocket(socket_temp);

​     WSACleanup();

​     return;

  }

  int len = sizeof(temp_addr);

  if (listen(socket_temp, 5) != 0) {

​     cout << "监听失败" << endl;

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

string getIP() //获取本机IP地址

{

  WSADATA WSAData;

  char hostName[256];

  if (!WSAStartup(MAKEWORD(2, 0), &WSAData))

  {

​     if (!gethostname(hostName, sizeof(hostName)))

​     {

​       hostent* host = gethostbyname(hostName);

​       if (host != NULL)

​       {

​         return inet_ntoa(*(struct in_addr*)*host->h_addr_list);

​       }

​     }

  }

  return "Get IP failed.";

}

string for_ls_ip(string ip) {

  int i = strlen(ip.c_str());

  int j = 0;

  while (j < i) {

​     if (ip[j] == '.')

​       ip[j] = ',';

​     j++;

  }

  return ip;

}

![img](D:\screen shout\typora\clip_image034.png)

![img](D:\screen shout\typora\clip_image036.jpg)

## 5、Get

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

​     cout << "临时端口绑定失败" << endl;

​     closesocket(socket_temp);

​     WSACleanup();

​     return;

  }

  int len = sizeof(temp_addr);

  if (listen(socket_temp, 5) != 0) {

​     cout << "监听失败" << endl;

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

if (send_buf == "get" && socket_flag == 1) {

​         mode_port = 2;

​         string port_part1, port_part2;

​         int port_temp = port_port;

​         string ip = for_ls_ip(getIP()) + ',';

​         itoa(htons(port_temp) & 0xff, (char*)port_part1.c_str(), 10);

​         itoa((htons(port_temp) >> 8) & 0xff, (char*)port_part2.c_str(), 10);

​         send_buf = "PORT " + ip + port_part1.c_str() + "," + port_part2.c_str() + "\r\n";

​         send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);

​         cin_flag = 1;

​       }

​       if (send_buf == "get" && socket_flag == 0) {

​         cout << "未连接" << endl;

​         cin_flag = 0;

​       }

![get.drawio](D:\screen shout\typora\clip_image038.png)

![img](D:\screen shout\typora\clip_image040.jpg)

## 6、Put

if (send_buf == "put" && socket_flag == 1) {

​         mode_port = 3;

​         string port_part1, port_part2;

​         int port_temp = port_port;

​         string ip = for_ls_ip(getIP()) + ',';

​         itoa(htons(port_temp) & 0xff, (char*)port_part1.c_str(), 10);

​         itoa((htons(port_temp) >> 8) & 0xff, (char*)port_part2.c_str(), 10);

​         send_buf = "PORT " + ip + port_part1.c_str() + "," + port_part2.c_str() + "\r\n";

​         send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);

​         cin_flag = 1;

​       }

​       if (send_buf == "put" && socket_flag == 0) {

​         cout << "未连接" << endl;

​         cin_flag = 0;

​       }

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

​     cout << "临时端口绑定失败" << endl;

​     closesocket(socket_temp);

​     WSACleanup();

​     return;

  }

  int len = sizeof(temp_addr);

  if (listen(socket_temp, 5) != 0) {

​     cout << "监听失败" << endl;

  }

  socket_temp1 = accept(socket_temp, (sockaddr*)&temp_addr, &len);

}

void put_send() {

  ifstream ifs;

  ofstream ofs;

  char put_buf[1024];

  memset(put_buf, '\0', sizeof(put_buf));

  file_name = "E:\\FTP_send_file\\" + file_name; //通用 C:\\Users\\

  ifs.open(file_name, ios::app);

  ifs >> put_buf;

  send(socket_temp1, put_buf, strlen(put_buf), 0);

  closesocket(socket_temp1);//连接完后要即使关闭套接字

}

![put.drawio](D:\screen shout\typora\clip_image042.png)

![img](D:\screen shout\typora\clip_image044.jpg)
 7、quit**
**   if (send_buf == "quit" && socket_flag == 1) {

​         send_buf = "QUIT\r\n";

​         send(seversocket, send_buf.c_str(), strlen(send_buf.c_str()), 0);

​         cout << "连接断开" << endl;

​         closesocket(seversocket);

​         WSACleanup();

​         cin_flag = 1;

​         socket_flag = 0;

​       }

​       if (send_buf == "quit" && socket_flag == 0) {

​         cout << "未连接" << endl;

​         cin_flag = 0;

​       }

 

case 221: //结束返回码

​     cin_flag = 0;

​     break;

![quit.drawio](D:\screen shout\typora\clip_image045.png)

![img](D:\screen shout\typora\clip_image047.jpg)

# 四．课程设计小结

这次课设加深了我对事件通知模型的理解深度，同时，也是我第一次抓包来分析服务端通信的规则，这让我既兴奋又有点害怕。考虑到本机的网络环境复杂，且FTP客户端与FTP服务端在同一台机子上会导致wireshark抓包分析困难，所以，我把FTP服务端安装在虚拟机上，方便分析，且也符合实际，因为服务端与客户端一般不在同一台机子上。度过开始的探索期，分析报文的格式，后面几乎毫无阻碍，除了在port模式的端口上分析了好久，得知前数字*256+后数字就是客户端向服务器开放的端口号。在完成基础功能后，自动获取IP地址与随机端口号也随之加上去，使其的作用范围不局限于我的电脑。总而言之，这次课程设计是我受益匪浅。

 

 