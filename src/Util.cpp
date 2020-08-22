#include <climits>
//
// Created by Haluk AKGUNDUZ on 02/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <miniz/miniz.h>
#include "Util.h"

std::string Util::hex2str(const uint8_t *in, int len) {

    static const char *const lut = "0123456789ABCDEF";

    std::string output;

    output.reserve((uint32_t) 2 * len);

    for (size_t i = 0; i < len; ++i) {

        output.push_back(lut[in[i] >> 4]);

        output.push_back(lut[in[i] & 15]);
    }

    return output;
}

bool Util::str2hex(uint8_t *out, const char *in, uint32_t len) {

    static const char *const lut = "0123456789ABCDEF";

    if (len & 1u) {
        return false;
    };

    for (size_t i = 0; i < len; i++) {

        char a = (char) toupper(in[i * 2]);

        const char *p = std::lower_bound(lut, lut + 16, a);

        if (*p != a) {

            return false;
        };

        char b = (char) toupper(in[i * 2 + 1]);

        const char *q = std::lower_bound(lut, lut + 16, b);

        if (*q != b) {

            return false;
        };

        out[i] = (uint8_t) (((p - lut) << 4u) | (q - lut));
    }

    return true;
}

std::string Util::parsePath(const std::filesystem::path& path, const std::string &str) {

    unsigned long len = strlen(ROOT_SIGN);
    unsigned long pos = 0;
    std::string parsed = str;

    do {

        pos = parsed.find(ROOT_SIGN, pos);

        if (pos == std::string::npos) {
            break;
        }

        parsed.replace(pos, len, path);

    } while (true);

    return parsed;
}

void Util::replaceStr(std::string &main, const std::string &search,
                      const std::string &replace) {
    size_t pos = 0;

    while ((pos = main.find(search, pos)) != std::string::npos) {

        main.replace(pos, search.length(), replace);

        pos += replace.length();
    }
}

void Util::init() {

    DIR *unixDir = opendir(UNIXSOCKET_PATH);
    if (!unixDir) {
        printf("Can not open unix socket path!!!");
        return;
    }

    dirent *entry;
    char path[300];

    while ((entry = readdir(unixDir)) != nullptr) {

        if (strncmp(entry->d_name, UNIXSOCKET_FILE_PREFIX, strlen(UNIXSOCKET_FILE_PREFIX)) == 0) {
            sprintf(path, "%s%s", UNIXSOCKET_PATH, entry->d_name);
            unlink(path);
        }
    }

    closedir(unixDir);
}
