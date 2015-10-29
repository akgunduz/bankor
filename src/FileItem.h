//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef __FileContent_H_
#define __FileContent_H_

#include "ContentItem.h"
#include "Unit.h"

class FileItem : public ContentItem {

    long id;
	char fileName[PATH_MAX];
	char rootPath[PATH_MAX];
//	char absPath[PATH_MAX];
//	char md5Path[PATH_MAX];
	uint8_t md5[MD5_DIGEST_LENGTH];
	bool flaggedToSent;
	FILETYPE fileType;

    void setRootPath(const char* rootPath);

public:
	FileItem(const char* rootPath);
    FileItem(FileItem*);
	FileItem(const char* rootPath,
             const char *fileName, FILETYPE fileType,
             int id = 0, uint8_t *md5 = nullptr);
	~FileItem(){};
    void set(const char *rootPath, const char *fileName,
             FILETYPE fileType, int id, uint8_t *md5);
    void set(FileItem*);
	bool isFlaggedToSent();
	void setFlaggedToSent(bool);
	FILETYPE getFileType();
	uint8_t* getMD5();
	void setMD5(uint8_t*);
    const char* getFileName();
    const char* getRootPath();
    long getID();
    void setID(long id);

    void setFile(const char* rootPath,
                 const char *fileName, FILETYPE fileType);
    std::string getAbsPath(Unit host, Unit node);
    std::string getAbsPath();
    std::string getMD5Path();

	virtual CONTENT_TYPES getType();
};

#endif //__Content_H_
