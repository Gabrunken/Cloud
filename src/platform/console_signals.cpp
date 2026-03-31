#include <platform/console_signals.hpp>

#include <assert.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <signal.h>
#endif

#ifdef _WIN32
static BOOL WINAPI ConsoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT || signal == CTRL_CLOSE_EVENT)
    {
        exit(0);
        return TRUE;
    }

    return FALSE;
}
#endif

void SetupSignalHandlers()
{
    #ifdef _WIN32
    if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE))
        assert(false);
    #else
    struct sigaction action;
    action.sa_handler = [](int signum){exit(0);};
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGINT, &action, nullptr);  //Ctrl+C
    sigaction(SIGTERM, &action, nullptr); //Kill request
    sigaction(SIGHUP, &action, nullptr);  //Close terminal
    #endif
}