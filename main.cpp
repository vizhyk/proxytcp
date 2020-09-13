#include "Proxy.hpp"

// usage
// ./proxy [port:forward_from] [port:forward_to]

void TrackingModeFlow()
{


}


int main(int argc, char** argv)
{

    Proxy::ForwardingData fwd {};


    // it holds address of a function that depends on mode choosen by user.[forwarding/tracking(extension A)/Ban(Extension B)
    Proxy::FunctionPointer fptr = nullptr;

    Proxy::ParseInputArguments(argc,argv,fwd,&fptr);


    printf("address of function in main fptr() is :%p\n", fptr);
    fptr(fwd);

    return 0;
}
