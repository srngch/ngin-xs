#include "Autoindex.hpp"

Autoindex::Autoindex(const std::string &filePath, const std::string &uri)
	: filePath_(filePath), uri_(uri) {
	DIR	*dir;

	dir = opendir(filePath_.c_str());
	if (!dir)
		throw std::runtime_error("autoindex: Directory opening failed");
	setAutoindexes(dir);
	closedir(dir);
	std::sort(autoindexes_.begin(), autoindexes_.end(), comp);
	setHtml();
}

Autoindex::~Autoindex() {}

ft_bool Autoindex::comp(AutoindexData &a1, AutoindexData &a2) {
	if ((a1.getIsDirectory() && a2.getIsDirectory())
		|| (!a1.getIsDirectory() && !a2.getIsDirectory())) {
		return a1.getName() < a2.getName();
		}
	else
		return a1.getIsDirectory() > a2.getIsDirectory();
}

void Autoindex::setAutoindexes(DIR *dir) {
	struct dirent	*entry;
	AutoindexData	ai;
	std::string		fullPath;
	struct stat		fileStat;
	struct tm		*tm;
	char			dateBuf[20];

	while ((entry = readdir(dir)) != nullptr) {
		ai.setName(entry->d_name);
		ai.setIsDirectory(FT_FALSE);
		fullPath = filePath_ + "/" + entry->d_name;
		if (isDirectory(fullPath)) {
			ai.setName(ai.getName() + "/");
			ai.setIsDirectory(FT_TRUE);
		}
		if (ai.getName().length() > 50) {
			ai.setName(ai.getName().substr(0, 47));
			ai.setName(ai.getName() + "..>");
		}
		stat(fullPath.c_str(), &fileStat);
		tm = gmtime(&fileStat.st_mtime);
		strftime(dateBuf, 20, "%d-%b-%Y %H:%M", tm);
		ai.setDate(dateBuf);
		ai.setSize(fileStat.st_size);
		ai.setPath(uri_ + entry->d_name);
		if (ai.getIsDirectory())
			ai.setPath(ai.getPath() + "/");
		if (ai.getName() != "./")
			autoindexes_.push_back(ai);
	}
}

void Autoindex::setHtml() {
	std::vector<AutoindexData>::iterator	it;

	html_ += "<html> \
			<head> \
				<title>Index of " + uri_ + "</title> \
			</head> \
			<body> \
				<h1>Index of " + uri_ + "</h1> \
				<hr> \
				<pre>";
	for (it = autoindexes_.begin(); it != autoindexes_.end(); it++) {
		html_ += "<a href=\"" + it->getPath() + "\">" \
			+ it->getName() + "</a>" \
			+ createPadding(50, it->getName().length()) + " " \
			+ it->getDate() + " ";
		if (it->getIsDirectory())
			html_ += createPaddedString(19, "-");
		else
			html_ += createPaddedString(19, ntos(it->getSize()));
		html_ += "\n";
	}
	html_ += "</pre> \
				<hr> \
			</body> \
		</html>";
}

std::string	Autoindex::getHtml() {
	return html_;
}
