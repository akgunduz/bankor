//
// Created by Haluk AKGUNDUZ on 22/06/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#include <openssl/md5.h>
#include "Message.h"

Message::Message(Unit host, const char* rootPath)
		: BaseMessage(host) {

    strcpy(this->rootPath, rootPath);
	job = nullptr;
}

Message::Message(Unit owner, int type, const char* rootPath)
		: BaseMessage(owner, type) {

    strcpy(this->rootPath, rootPath);
    job = nullptr;
}

const char *Message::getRootPath() {
	return rootPath;
}

void Message::setRootPath(const char *rootPath) {
    strcpy(this->rootPath, rootPath);
}

Job* Message::getJob() {
    return job;
}

void Message::setJob(int streamFlag, Job *job) {

    setStreamFlag(streamFlag);

    this->job = job;
}

bool Message::readMD5(int desc, uint8_t *md5) {

	if (!readBlock(desc, md5, MD5_DIGEST_LENGTH)) {
		LOG_E("Can not read MD5 from stream");
		return false;
	}

	return true;
}

bool Message::readJobID(int desc, char* jobDir, struct BlockHeader* header) {

    if (header->blockType != BLOCK_FILE_JOB) {
        LOG_E("readJobID can not read other blocks");
        return false;
    }

    if (!readString(desc, jobDir, header->sizes[0])) {
        LOG_E("readJobID can not read path data");
        return false;
    }

    return true;
}

bool Message::readFileBinary(int desc, FileItem *content, struct BlockHeader *header) {

	if (header->blockType != BLOCK_FILE_BINARY) {
		LOG_E("readFileBinary can not read other blocks");
		return false;
	}

	char path[PATH_MAX];
	if (!readString(desc, path, header->sizes[0])) {
		LOG_E("readFileBinary can not read path data");
		return false;
	}

    long fileType;
    if (!readNumber(desc, &fileType)) {
        LOG_E("readFileBinary can not read filetype data");
        return false;
    }

    long id;
    if (!readNumber(desc, &id)) {
        LOG_E("readFileBinary can not read filetype data");
        return false;
    }

    content->setFile(getRootPath(), path, (FILETYPE)fileType);
    content->setID(id);

    if (header->sizes[1] != MD5_DIGEST_LENGTH) {
        LOG_E("Md5 size must be %d long", MD5_DIGEST_LENGTH);
        return false;
    }

    uint8_t calcMD5[MD5_DIGEST_LENGTH];
	if (!readBinary(desc, content->getAbsPath(getHost(), getOwner()).c_str(), calcMD5, content->getMD5Path().c_str(), header->sizes[2])) {
		LOG_E("readFileBinary can not read Binary data");
		return false;
	}

    uint8_t md5[MD5_DIGEST_LENGTH];
    if (!readMD5(desc, md5)) {
        LOG_E("readFileBinary can not read MD5 data");
        return false;
    }

	if (memcmp(md5, calcMD5, MD5_DIGEST_LENGTH) != 0) {
		LOG_E("readFileBinary mismatch in md5 data");
		return false;
	}

    content->setMD5(md5);
	content->setValid(true);

	return true;
}

bool Message::readFileMD5(int desc, MD5Wrapper *content, struct BlockHeader* header) {

	if (header->sizes[0] != MD5_DIGEST_LENGTH) {
		LOG_E("Md5 size must be %d long", MD5_DIGEST_LENGTH);
		return false;
	}

	if (!readMD5(desc, content->md5)) {
		LOG_E("Can not read md5");
		return false;
	}

	return true;
}

bool Message::readMessageBlock(int in, BlockHeader *blockHeader) {

    switch(blockHeader->blockType) {

        case BLOCK_FILE_JOB:
            char jobDir[PATH_MAX];
            if (!readJobID(in, jobDir, blockHeader)) {
                return false;
            }

            if (job != nullptr) {
                delete job;
            }

            job = new Job(getRootPath(), jobDir);

            setRootPath(job->getRootPath());
            break;

        case BLOCK_FILE_BINARY: {

            FileItem *fileItem = new FileItem(getRootPath());

            if (!readFileBinary(in, fileItem, blockHeader)) {
                return false;
            }

            if (fileItem->getFileType() == FILE_JOB) {

                job->set(fileItem);

            } else {

                delete fileItem;
            }
        }
            break;

        case BLOCK_FILE_MD5: {

            MD5Wrapper md5;
            if (!readFileMD5(in, &md5, blockHeader)) {
                return false;
            }

            md5List.push_back(md5);
        }
            break;

        default:
            return false;
    }

    return true;
}

bool Message::writeMD5(int desc, uint8_t *md5) {

	if (!writeBlock(desc, md5, MD5_DIGEST_LENGTH)) {
		LOG_E("Can not write md5 to stream");
		return false;
	}

	return true;
}

bool Message::writeFileBinary(int desc, FileItem *content) {

	BlockHeader blockHeader = {3, BLOCK_FILE_BINARY};

    blockHeader.sizes[0] = (uint32_t)strlen(content->getFileName());
    blockHeader.sizes[1] = MD5_DIGEST_LENGTH;
    blockHeader.sizes[2] = getBinarySize(content->getAbsPath(getHost(), getOwner()).c_str());

	if (!writeBlockHeader(desc, &blockHeader)) {
		return false;
	}

	if (!writeString(desc, content->getFileName())) {
        return false;
    }

    if (!writeNumber(desc, content->getFileType())) {
        return false;
    }

    if (!writeNumber(desc, content->getID())) {
        return false;
    }

	uint8_t calcMD5[MD5_DIGEST_LENGTH];
	if (!writeBinary(desc, content->getAbsPath(getHost(), getOwner()).c_str(), calcMD5)) {
		LOG_E("writeFileBinary can not write Binary data");
		return false;
	}

    content->setMD5(calcMD5);

	if (!writeMD5(desc, calcMD5)) {
        LOG_E("writeFileBinary can not write md5 data");
		return false;
	}

	return true;

}

bool Message::writeFileJob(int desc, Job *job) {

	BlockHeader blockHeader = {1, BLOCK_FILE_JOB};

    blockHeader.sizes[0] = (int) strlen(job->getJobDir());

	if (!writeBlockHeader(desc, &blockHeader)) {
		return false;
	}

	return writeString(desc, job->getJobDir());
}

bool Message::writeFileMD5(int desc, FileItem *content) {

    BlockHeader blockHeader = {1, BLOCK_FILE_MD5};

    blockHeader.sizes[0] = MD5_DIGEST_LENGTH;

    if (!writeBlockHeader(desc, &blockHeader)) {
        return false;
    }

    return writeMD5(desc, content->getMD5());
}

bool Message::writeMessageStream(int out, int streamFlag) {

    switch(streamFlag) {

        case STREAM_RULE:

            if (!writeFileJob(out, job)) {
                return false;
            }

            if (!writeFileBinary(out, job)) {
                return false;
            }

            for (int i = 0; i < job->getContentCount(CONTENT_FILE); i++) {
                FileItem *content = (FileItem *)job->getContent(CONTENT_FILE, i);
                if (!writeFileBinary(out, content)) {
                    return false;
                }
            }

            break;

        case STREAM_BINARY:

            for (int j = 0; j < job->getContentCount(CONTENT_FILE); j++) {
                Rule *rule = (Rule *)job->getContent(CONTENT_FILE, j);
                for (int i = 0; i < rule->getContentCount(CONTENT_FILE); i++) {
                    FileItem *content = (FileItem *)rule->getContent(CONTENT_FILE, i);
                    if (content->isFlaggedToSent()) {
                        if (!writeFileBinary(out, content)) {
                            return false;
                        }
                    }
                }
            }
            break;

        case STREAM_MD5ONLY:
            for (int j = 0; j < job->getContentCount(CONTENT_FILE); j++) {
                Rule *rule = (Rule *)job->getContent(CONTENT_FILE, j);
                for (int i = 0; i < rule->getContentCount(CONTENT_FILE); i++) {
                    FileItem *content = (FileItem *)rule->getContent(CONTENT_FILE, i);
                    if (content->isFlaggedToSent()) {
                        if (!writeFileMD5(out, content)) {
                            return false;
                        }
                    }
                }
            }
            break;

        default :
            break;
    }

    return true;

}


