#include "../cLog/loglite.h"

extern int log_init(const char* config,const char* moduleName);

int main(int argc, char** argv)
{
    log_init("./log.config",argv[0]);

    LOGW(MODULE_A, "%s--%d", "\n", "Hello world", 1);
    LOGE(MODULE_B, "%s--%d", "\n", "Hello world", 2);

    char* test = "Hello world";

    A_LOGI(test);
    A_LOGW("%s--%d", test, 4);

    log_drop();

    return 0;
}
