#include <NetworkingCore/MessengerServer.h>

#define LOOPBACK

int main(int argc, char* argv[])
{
#ifdef LOOPBACK
  const char* ip_str = "127.0.0.1";
#else
  // From Windows docs: "if the IP string is an emtpy string, a registered address 
  // on the local computer is returned", which is exactly what we want for the 
  // server.
  const char* ip_str = NULL;
#endif

  // Port number from MS documentation.
  auto const port_num_str = "27015";

  MessengerServer server(ip_str, port_num_str, 5);
  server.exec();

  return 0;
}
