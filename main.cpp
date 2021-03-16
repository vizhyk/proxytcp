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

    Proxy::Status status;
    Proxy::CommandLineOptions cmd(argc, argv);
    Proxy::Application application(cmd.GetChosenPort());

    status = application.InitConnectionManager(cmd.GetChosenExecutionModeID(), cmd.GetChosenOutputFilePath());

    if(status.Failed())
        return status.Code();

    return application.Run().Code();
}

