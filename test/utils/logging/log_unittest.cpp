#include "log.h"

using namespace share_me_utils;

int main(int argc, char *argv[]) {
    Log::Instance()->SetPrefix("%d %t %F %f %l");
    LOG_TRACE("LOG TRACE");
    LOG_DEBUG("LOG DEBUG");
    LOG_INFO("LOG INFO");
    LOG_ERROR("LOG ERROR");
    LOG_FATAL("LOG FATAL");
    return 0;
}