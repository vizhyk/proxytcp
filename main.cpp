#include <csignal>
#include <cstdlib>
#include <iostream>
#include "src/Application/Application.hpp"

void signalHandler( int signum ) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";

    // cleanup and close up stuff here
    // terminate program

    exit(signum);
}

int main(int argc, char** argv)
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    Proxy::Application a;
    return a.Run();
}

