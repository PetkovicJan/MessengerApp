#include <NetworkingCore/MessengerServer.h>

#define LOOPBACK

int main(int argc, char* argv[])
{
#ifdef LOOPBACK
  // Loopback address: Packets sent to this address never reach the network, but
  // are looped through the network interface card only. This is useful for 
  // testing and development, when running server and client on the same machine.
  const char* ipStr = "127.0.0.1";
#else
  // From Windows docs: "if the IP string is an emtpy string, a registered address 
  // on the local computer is returned", which is exactly what we want for the 
  // server if we want to expose it to the outside world.
  const char* ipStr = NULL;
#endif

  // Port number from MS documentation. Note that it could be any of unused ports.
  char const* portNumStr = "27015";
  constexpr int MaxNumberOfClients = 10;
  MessengerServer server(ipStr, portNumStr, MaxNumberOfClients);

  server.exec();

  return 0;
}
