#include "Dispatcher/clientdispatcher.h"
#include "notepad.h"

int main(int argc, char *argv[])
{
    Symposium::clientdispatcher cl;
    return cl.run(argc,argv);
}
