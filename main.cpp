#include <csignal>
#include <cstdlib>
#include <iostream>

#include "Application/Application.hpp"
#include "CommandLineOptions/CommandLineOptions.hpp"

void signalHandler( int signum )
{
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    exit(signum);
}

int main(int argc, char** argv)
{
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    Proxy::CommandLineOptions cmd(argc, argv);

    Proxy::Application application(cmd.GetChosenExecutionMode(), cmd.GetChosenPort(), cmd.GetChosenOutputFilePath());

    return application.Run().Code();
}

