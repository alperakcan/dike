
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#define OPTION_HELP     'h'
#define OPTION_TRACK    't'
#define OPTION_RECORD   'r'

static struct option longopts[] = {
        { "help",       no_argument,            NULL,   OPTION_HELP },
        { "track",      required_argument,      NULL,   OPTION_TRACK },
        { "record",     required_argument,      NULL,   OPTION_RECORD },
};

int main (int argc, char *argv[])
{
        (void) argc;
        (void) argv;
        return 0;
}
