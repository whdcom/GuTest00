#include <iostream>
#include <Windows.h>
#include <string.h>
#include <string>
#include <algorithm>

//#include <Gtest.h>
using namespace std;

//int Gtest1();
//#define BUF_SIZE 147455
#define BUF_SIZE 245759
void Gpipe(char* m);
//using std::cerr;
//using std::cout;
//using std::endl;
const char pipeName[] = "\\\\.\\pipe\\test_pipe";
void Gpipe(char* m)
{   
    //Gtest1();
    HANDLE h_pipe;
    char buf_msg[] = "27.891";
    DWORD num_rcv; //实际接收到的字节数
    cout << "Try to connect named pipe..." << endl;
    //连接命名管道
    //遇到的问题pipeName不兼容，原因在于配置时，字符集为unicode，应该改成多字节字符集
    if (WaitNamedPipe("\\\\.\\pipe\\test_pipe", NMPWAIT_WAIT_FOREVER))
    {
        //打开指定命名管道
        h_pipe = CreateFile("\\\\.\\pipe\\test_pipe", GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h_pipe == INVALID_HANDLE_VALUE)
        {
            cerr << "Failed to open the appointed named pipe!Error code: " << ::GetLastError() << "\n";
            system("pause");
            return ;
        }
        else
        {
            cout << h_pipe << endl;
            if (WriteFile(h_pipe, m, BUF_SIZE, &num_rcv, nullptr))
            {
                cout << "Message sent successfully..." << endl;
                
            }
            else
            {
                cerr << "Failed to send message!Error code: " << ::GetLastError() << endl;
                CloseHandle(h_pipe);
                system("pause");
                return ;
            }
        }
        CloseHandle(h_pipe);
    }

}