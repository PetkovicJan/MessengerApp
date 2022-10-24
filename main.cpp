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

  auto const ip_str = "TODO";
  auto const port_num_str = "TODO";

  addrinfo host_info;
  ZeroMemory(&host_info, sizeof(host_info));
  host_info.ai_family = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_protocol = IPPROTO_TCP;

  addrinfo* address_info;
  if (getaddrinfo(ip_str, port_num_str, &host_info, &address_info) != 0)
  {
    std::cout << "Get address info failed.\n";
    WSACleanup();
    return 1;
  }

  SOCKET socket_handle = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
  if (socket_handle == INVALID_SOCKET)
  {
    std::cout << "Creating socket failed.\n";
    freeaddrinfo(address_info);
    WSACleanup();
    return 1;
  }

  if (WSACleanup() != 0)
  {
    std::cout << "Cleanup failed.\n";
    return 1;
  }

  return 0;
}