#include "config/config.h"

int main(int argc, char **argv)
{
    string user = "root";
    string passwd = "Quenkar";
    string databasename = "yourdb";

    Config config;
    config.parse_arg(argc, argv);

    WebServer webserver;

    webserver.init(config.PORT, user, passwd, databasename, config.LOGWrite,
                   config.OPT_LINGER, config.TRIGMode, config.sql_num, config.thread_num,
                   config.close_log, config.actor_model);

    webserver.log_write();

    webserver.sql_pool();

    webserver.thread_pool();

    webserver.trig_mode();

    webserver.event_listen();

    webserver.event_loop();

    return 0;
}