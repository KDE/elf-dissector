/*
    Copyright (C) 2015 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int usage()
{
    fprintf(stderr, "Usage: ldbenchark-runner [RTLD_LAZY|RTLD_NOW] <files>\n");
    return 1;
}

int main(int argc, char **argv)
{
    if (argc < 3)
        return usage();

    int flags = 0;
    if (strcmp(argv[1], "RTLD_NOW") == 0)
        flags = RTLD_NOW;
    else if (strcmp(argv[1], "RTLD_LAZY") == 0)
        flags = RTLD_LAZY;
    else
        return usage();

    for (int i = 2; i < argc; ++i) {
        if (dlopen(argv[i], flags | RTLD_NOLOAD) != NULL) {
            fprintf(stderr, "%s is already loaded, check argument order!\n", argv[i]);
            continue;
        }

        struct timespec start, end;
        clock_gettime(CLOCK_REALTIME, &start);
        void* result = dlopen(argv[i], flags);
        clock_gettime(CLOCK_REALTIME, &end);

        if (!result) {
            fprintf(stderr, "Loading %s failed: %s\n", argv[i], dlerror());
            return 1;
        }

        long long diff = (end.tv_sec - start.tv_sec) * 1000000000;
        if (diff == 0)
            diff = end.tv_nsec - start.tv_nsec;
        else {
            diff += end.tv_nsec;
            diff += 1000000000 - start.tv_nsec;
        }
        fprintf(stdout, "LDBENCHMARKRUNNER\t%s\t%.2f\n", argv[i], diff/1000.0);
    }

    return 0;
}
