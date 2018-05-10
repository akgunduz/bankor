//
// Created by akgunduz on 23.10.2015.
//

#include "Job.h"
#include "ParameterItem.h"
#include "ExecutorItem.h"

Job::Job(ComponentObject host, const char* jobDir)
        : JsonItem(host, jobDir, JOB_FILE){

    init();

}

Job::~Job() {

}

void Job::init() {

    contentTypes[CONTENT_NAME] = new JsonType(CONTENT_NAME, "name", this, parseNameNode);
    contentTypes[CONTENT_FILE] = new JsonType(CONTENT_FILE, "files", this, parseFileNode);
    contentTypes[CONTENT_PARAM] = new JsonType(CONTENT_PARAM, "parameters", this, parseParamNode);
    contentTypes[CONTENT_EXECUTOR] = new JsonType(CONTENT_EXECUTOR, "executors", this, parseExecutorNode);

    if (!parse()) {
        LOGS_E(getHost(), "Job could not parsed!!!");
        return;
    }

    for (int i = 0; i < getExecutorCount(); i++) {
        getExecutor(i)->parse(this);
    }

    if (!createDependencyMap()){
        LOGS_E(getHost(), "Dependency Loop Detected in the Job!!!");
        return;
    }

}

bool Job::parseNameNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_string) {
        LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Name Node");
        return false;
    }

    const char *name = json_object_get_string(node);

    ((Job*)parent)->setName(name);

    return true;
}

bool Job::parseFileNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Files Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Files Node");
            return false;
        }

        const char* path = json_object_get_string(child);

        auto *content = new FileItem(((Job*)parent)->getHost(),
                                     ((Job*)parent)->getJobDir(), path, i);

        content->validate();

        ((Job*)parent)->contentList[CONTENT_FILE].push_back(content);

    }
    return true;
}

bool Job::parseParamNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Parameter Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Parameter Node");
            return false;
        }

        const char *param = json_object_get_string(child);

        auto *content = new ParameterItem(param);

        ((Job*)parent)->contentList[CONTENT_PARAM].push_back(content);
    }

    return true;
}

bool Job::parseExecutorNode(JsonItem *parent, json_object *node) {

    enum json_type type = json_object_get_type(node);
    if (type != json_type_array) {
        LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Executor Node");
        return false;
    }

    for (unsigned int i = 0; i < json_object_array_length(node); i++) {
        json_object *child = json_object_array_get_idx(node, i);

        type = json_object_get_type(child);
        if (type != json_type_string) {
            LOGS_E(((Job*)parent)->getHost(), "Invalid JSON Executor Node");
            return false;
        }

        const char *exec = json_object_get_string(child);

        auto *content = new ExecutorItem(exec);

        ((Job*)parent)->contentList[CONTENT_EXECUTOR].push_back(content);
    }

    return true;
}

Uuid Job::getJobID() {

    return id;
}

const char *Job::getName() {

    return name;
}

void Job::setName(const char *name) {

    strncpy(this->name, name, 50);
}

int Job::getExecutorCount() {

    return getContentCount(CONTENT_EXECUTOR);
}

ExecutorItem* Job::getExecutor(int index) {

    return (ExecutorItem*)getContent(CONTENT_EXECUTOR, index);
}

int Job::getFileCount() {

    return getContentCount(CONTENT_FILE);
}

FileItem* Job::getFile(int index) {

    return (FileItem*)getContent(CONTENT_FILE, index);
}

long Job::getOrderedCount() {

    return orderedList.size();
}

ExecutorInfo Job::getOrdered(int index) {

    return orderedList[index];
}

ExecutorItem* Job::getOrderedExecution(int index) {

    return orderedList[index].get();
}

PROCESS_STATE Job::getOrderedState(int index) {

    return orderedList[index].getState();
}

void Job::setOrderedState(int index, PROCESS_STATE state) {

    orderedList[index].setState(state);
}

ExecutorInfo Job::getUnServed() {

    for (int i = 0; i < getOrderedCount(); i++) {

        if (getOrderedState(i) == PROCESS_STATE_STARTED ||
            getOrderedState(i) == PROCESS_STATE_ENDED) {
            continue;
        }

        if (!getOrderedExecution(i)->isValid()) {
            continue;
        }

        setOrderedState(i, PROCESS_STATE_STARTED);

        return getOrdered(i);
    }

    return ExecutorInfo(0, NULL);
}

long Job::getUnServedCount() {

    int count = 0;

    for (int i = 0; i < getOrderedCount(); i++) {

        if (getOrderedState(i) == PROCESS_STATE_STARTED ||
            getOrderedState(i) == PROCESS_STATE_ENDED) {
            continue;
        }

        if (!getOrderedExecution(i)->isValid()) {
            continue;
        }

        count++;
    }

    return count;
}

long Job::getProvisionCount() {

    int count = 0;

    for (int i = 0; i < getOrderedCount(); i++) {

        if (getOrderedState(i) == PROCESS_STATE_STARTED ||
            getOrderedState(i) == PROCESS_STATE_ENDED) {
            continue;
        }

        if (!getOrderedExecution(i)->isValid()) {
            continue;
        }

        if (getOrderedState(i) == PROCESS_STATE_NOTSTARTED) {

            setOrderedState(i, PROCESS_STATE_PROVISION);
            count++;
        }
    }

    return count;
}

bool Job::isEnded() {

    for (int i = 0; i < getOrderedCount(); i++) {

        if (getOrderedState(i) != PROCESS_STATE_ENDED) {
            return false;
        }
    }

    return true;
}

ExecutorItem *Job::getByOutput(int index) {

    for (int i = 0; i < getExecutorCount(); i++) {

        auto *executor = getExecutor(i);

        TypeFileInfoList list = FileInfo::getFileList(executor->getFileList(), true);
        if (list.empty()) {
            continue;
        }

        if (list[0].get()->getID() == index) {
            return executor;
        }
    }

    return NULL;
}

bool Job::createDependencyMap() {

    int depth[getFileCount()];
    std::vector<int> adj[getFileCount()];
    std::list<int> initial, final;

    bzero(depth, (size_t)getFileCount() * sizeof(int));

    for (int i = 0; i < getExecutorCount(); i++) {

        TypeFileList outList, depList;

        TypeFileInfoList *fileList = getExecutor(i)->getFileList();

        for (int j = 0; j < fileList->size(); j++) {

            FileItem* file = fileList->at(j).get();

            fileList->at(j).isOutput() ? outList.push_back(file) : depList.push_back(file);
        }

        int id = outList[0]->getID();

        for (int j = 0; j < depList.size(); j++) {

            //TODO will be updated with multi output files

            adj[depList[j]->getID()].push_back(id);

            depth[id]++;
        }
    }

    for (int i = 0; i < getFileCount(); i++) {

        if (depth[i] == 0) {
            initial.push_back(i);
        }
    }

    while(!initial.empty()) {

        int current = initial.front();
        initial.pop_front();

        final.push_back(current);

        for (int i = 0; i < adj[current].size(); i++) {

            depth[adj[current][i]] -= 1;
            if (depth[adj[current][i]] == 0) {
                initial.push_back(adj[current][i]);
            }
        }
    }

    for (int i = 0; i < getFileCount(); i++) {

        if (depth[i] > 0) {
            return false;
        }
    }

    orderedList.clear();

    for (int i : final) {

        auto *content = getByOutput(i);
        if (content) {
            orderedList.emplace_back(ExecutorInfo(orderedList.size(), content));
        }
    }

    return true;
}
