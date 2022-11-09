#include "AutoindexData.hpp"

AutoindexData::AutoindexData() {}

AutoindexData::~AutoindexData() {}

std::string AutoindexData::getName() {
	return name_;
}

std::string AutoindexData::getDate() {
	return date_;
}

std::size_t AutoindexData::getSize() {
	return size_;
}

std::string AutoindexData::getPath() {
	return path_;
}

ft_bool AutoindexData::getIsDirectory() {
	return isDirectory_;
}

void AutoindexData::setName(const std::string &name) {
	name_ = name;
}

void AutoindexData::setDate(const std::string &date) {
	date_ = date;
}

void AutoindexData::setSize(std::size_t size) {
	size_ = size;
}

void AutoindexData::setPath(const std::string &path) {
	path_ = path;
}

void AutoindexData::setIsDirectory(ft_bool isDirectory) {
	isDirectory_ = isDirectory;
}
