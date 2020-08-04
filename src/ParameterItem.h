//
// Created by Haluk Akgunduz on 14/07/14.
// Copyright (c) 2014 Haluk Akgunduz. All rights reserved.
//

#ifndef DICON_PARAMETERITEM_H
#define DICON_PARAMETERITEM_H

#include "ContentItem.h"

class ParameterItem : public ContentItem {
	char sPtr[NAME_MAX];
public:
	ParameterItem(const HostUnit&, long, long, const char *);
	~ParameterItem(){};
	const char* getParam();
	void setParam(const char*);
	virtual CONTENT_TYPES getType() const;

    virtual bool check();
};

#endif //DICON_PARAMETERITEM_H
