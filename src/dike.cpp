
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#define OPTION_HELP     'h'
#define OPTION_TRACK    't'
#define OPTION_RECORD   'r'

static struct option long_options[] = {
        { "help",       no_argument,            NULL,   OPTION_HELP },
        { "track",      required_argument,      NULL,   OPTION_TRACK },
        { "record",     required_argument,      NULL,   OPTION_RECORD },
};

static void print_usage (const char *pname)
{
        fprintf(stdout, "Usage: %s [OPTION]...", pname);
        fprintf(stdout, "  -t, --track  : base route to compare with\n");
        fprintf(stdout, "  -r, --record:: recorded route to calculate truth\n");
        fprintf(stdout, "  -h, --help   : this text\n");
}

int main (int argc, char *argv[])
{
        int o;
        int oi;

        const char *o_track;
        const char *o_record;

        while ((o = getopt_long(argc, argv, "ht:r:", long_options, &oi))) {
                switch (o) {
                        case 'h':
                                print_usage(argv[0]);
                                goto out;
                        case 't':
                                o_track = optarg;
                                break;
                        case 'r':
                                o_record = optarg;
                                break;
                }
        }

out:
        return 0;
}
