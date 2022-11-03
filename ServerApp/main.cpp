#include "../Server.h"
#include "../Messenger.h"

int main(int argc, char* argv[])
{
  // This machine's IP.
  auto const ip_str = "127.0.0.1";

  // Port number from MS documentation.
  auto const port_num_str = "27015";

  Server server(ip_str, port_num_str, 5);
  server.exec();

  return 0;
}
