//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_FILEITEM_H
#define DICON_FILEITEM_H

#include "ContentItem.h"

#define ARCH_PATH "bin"

class FileItem : public ContentItem {

	std::string name;
    std::filesystem::path parentPath;

    bool is_independent{false};
    bool is_exist[ARCH_MAX]{};
	bool is_required{};
	bool is_executable{};

    std::uintmax_t size[ARCH_MAX]{};

public:
	FileItem(const TypeHostUnit&, long, long = 0, std::string = "");
    explicit FileItem(const TypeHostUnit&, bool = false);
	FileItem(const TypeHostUnit&, long, std::filesystem::path, std::string);
	~FileItem() override = default;

    const std::string& getName();
    void setName(const std::string&);

    std::uintmax_t getSize(ARCH = ARCH_NONE);

	CONTENT_TYPES getType() override;

    bool check(ARCH = ARCH_NONE);

    bool required() const;
    void setRequired(bool);

    bool isIndependent() const;
    void setIndependent(bool);

    bool isExecutable() const;
    void setExecutable(bool);

    std::filesystem::path getParentPath(ARCH);
    std::filesystem::path getParentRefPath(ARCH);
    std::filesystem::path getPath(ARCH = ARCH_NONE);
    std::filesystem::path getRefPath(ARCH = ARCH_NONE);
};

typedef std::shared_ptr<FileItem> TypeFileItem;
typedef std::vector<TypeFileItem> TypeFileList;

#endif //DICON_FILEITEM_H
