#include <iostream>
#include <Windows.h>
#include <string.h>
#include <string>

using namespace std;


#define BUF_SIZE 6

//using std::cerr;
//using std::cout;
//using std::endl;
const char pipeName[]= "\\\\.\\pipe\\test_pipe";
int main()
{
    HANDLE h_pipe;
    char buf_msg[] = "27.891";
    DWORD num_rcv; //ʵ�ʽ��յ����ֽ���
    cout << "Try to connect named pipe..." << endl;
    //���������ܵ�
    //����������pipeName�����ݣ�ԭ����������ʱ���ַ���Ϊunicode��Ӧ�øĳɶ��ֽ��ַ���
    if (WaitNamedPipe("\\\\.\\pipe\\test_pipe", NMPWAIT_WAIT_FOREVER))
    {
        //��ָ�������ܵ�
        h_pipe = CreateFile("\\\\.\\pipe\\test_pipe", GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h_pipe == INVALID_HANDLE_VALUE)
        {
            cerr << "Failed to open the appointed named pipe!Error code: " << ::GetLastError() << "\n";
            system("pause");
            return 1;
        }
        else
        {
            cout << h_pipe << endl;
            if (WriteFile(h_pipe, buf_msg, BUF_SIZE, &num_rcv, nullptr))
            {
                cout << "Message sent successfully..."<<endl;
            }
            else
            {
                cerr << "Failed to send message!Error code: " << ::GetLastError() <<endl;
                CloseHandle(h_pipe);
                system("pause");
                return 1;
            }
        }
        CloseHandle(h_pipe);
    }
    ::system("pause");
    return 0;
}