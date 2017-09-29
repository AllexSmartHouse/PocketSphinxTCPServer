#include <QCoreApplication>

#include "pocketsphinx.h"
#include <sphinxbase/prim_type.h>
#include <cmd_ln.h>

#include "maintcpserver.h"

int main(int argc, char *argv[])
{
    cmd_ln_t* config = cmd_ln_parse_r(NULL, ps_args(), argc, argv, TRUE);
    //config= cmd_ln_init(NULL, ps_args(), TRUE, "-upperf", "4000", "-samprate", "8000", "-hmm", (path+"zero_ru.cd_semi_4000").toStdString().c_str(), "-dict", pathToGenDic.toStdString().c_str(), "-lm", (path+"ru.lm").toStdString().c_str(), "-remove_noise", "no"/*, "-logfn", "/dev/null"*/, NULL);

    QCoreApplication a(argc, argv);

    MainTCPServer server(config,9870);
    server.startServer();

    int r = a.exec();

    cmd_ln_free_r(config);

    return r;
}
