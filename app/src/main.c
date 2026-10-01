#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

// registers this file as the "app" log module, so you can run "log enable dbg app" in the shell
LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

int main(void)
{
    // nothing else is needed to get the shell running: it is started automatically
    // in its own thread by CONFIG_SHELL, and all of its commands come from prj.conf / Kconfig
    LOG_INF("Zephyr shell template started, press tab or type 'help'");

    // main is free to do your application's work here, the shell keeps running alongside it
    // use LOG_INF()/LOG_ERR() instead of printk() so output doesn't break the shell prompt
    return 0;
}
