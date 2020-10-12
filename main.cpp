/*      Quick Reference:
 *
 *      There're 3 modes according to the task:
 *          - Forwarding(item 3)
 *          - Tracking(item 4. Bonus A)
 *          - Ban(item 5. Bonus B)
 *
 *      There're following LONG options:
 *      --mode [0-2]
 *          0 - Forwarding
 *          1 - Tracking
 *          2 - Ban
 *      --domain [coolsite.io]
 *          Ban mode: is used as domain that is ALLOWED to connect
 *          Tracking: To track all client hello messages related to that  host.
 *      --ban [badsite.oi]
 *          Is Used only in Ban mode as a host that is NOT allowed to connect
 *      --lp [8081]
 *          listening port
 *      --dp [8080]
 *          destination port
 *
 *     There're configurations according to the task
 *
 *     Run ./compile.sh
 *     Forwarding:
 *          ./proxytcp --mode 0 --domain coolsite.io --lp 8081 --dp 8080
 *
 *     Tracking:
 *          ./proxytcp --mode 1 --lp 8080
 *          Then surf
 *
 *     Ban:
 *          ./proxytcp --mode 2 --domain coolsite.io --ban badsite.io --lp 8081 --dp 8080
 *
 */

#include "CommandLineOptions/CommandLineOptions.hpp"
#include "Application/Application.hpp"
#include "ExecutionModes/ExecutionMode.hpp"
#include "ConnectionInfo/ConnectionInfo.hpp"

int main(int argc, char** argv)
{
//    Proxy::CommandLineOptions cmdLineOptions(argc,argv);

//    const Proxy::ExecutionModes::ExecutionModeType mode = cmdLineOptions.GetExecutionModeType();
//    const Proxy::ConnectionInfo info = cmdLineOptions.GetConnectionInfo();

    Proxy::Application application(Proxy::ExecutionModes::ExecutionModeType::Forwarding , {8080,8081,"coolsite.io","badsite.io"});

    return application.Run();
}


