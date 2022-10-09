#include "Autoindex.hpp"

Autoindex::Autoindex() {}

Autoindex::~Autoindex() {}

void Autoindex::setName(const std::string &name) {
	name_ = name;
}

void Autoindex::setDate(const std::string &date) {
	date_ = date;
}

void Autoindex::setSize(std::size_t size) {
	size_ = size;
}

void Autoindex::setPath(const std::string &path) {
	path_ = path;
}

void Autoindex::setIsDirectory(ft_bool isDirectory) {
	isDirectory_ = isDirectory;
}


std::string Autoindex::getName() {
	return name_;
}

std::string Autoindex::getDate() {
	return date_;
}

std::size_t Autoindex::getSize() {
	return size_;
}

std::string Autoindex::getPath() {
	return path_;
}

ft_bool Autoindex::getIsDirectory() {
	return isDirectory_;
}

