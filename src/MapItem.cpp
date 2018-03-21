//
// Created by akgunduz on 02.11.2015.
//

#include "MapItem.h"

MapItem::MapItem(Unit host, const char *jobDir,
                 const char *fileName, FILETYPE fileType) {

    this->fileType = fileType;

    if(host.getType() == COMP_COLLECTOR && fileType == FILE_ARCH) {
        for (short i = 0; i < ARCH_MAX; i++) {
            files[i] = new FileItem(host, Unit(COMP_NODE, (ARCH) i), jobDir, fileName, fileType);
        }
        this->fileCount = ARCH_MAX;

    } else {
        files[0] = new FileItem(host, Unit(COMP_NODE), jobDir, fileName, fileType);
        this->fileCount = 1;
    }
}

CONTENT_TYPES MapItem::getType() {
    return CONTENT_MAP;
}

FileItem *MapItem::get() {

    return files[0];
}

FileItem *MapItem::get(int index) {

    if (index >= fileCount) {
        return nullptr;
    }

    return files[index];
}

FileItem *MapItem::get(ARCH arch) {

    if (fileType != FILE_ARCH || arch >= ARCH_MAX) {
        return files[0];
    }

    return files[arch];
}

int MapItem::getCount() {

    return fileCount;
}

FILETYPE MapItem::getFileType() {
    return fileType;
}

bool MapItem::isValid() {
    return true;
}
