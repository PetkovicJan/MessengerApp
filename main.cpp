#include <iostream>

#include <WinSock2.h>
#include <WS2tcpip.h>

int main(int argc, char* argv[])
{
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
  {
    std::cout << "Startup failed.\n";
    return 1;
  }

  if (WSACleanup() != 0)
  {
    std::cout << "Cleanup failed.\n";
    return 1;
  }

  return 0;
}