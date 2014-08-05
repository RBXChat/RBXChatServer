#ifndef _METAMANAGER_BAN_CPP
#define _METAMANAGER_BAN_CPP

#include "loadfiles.cpp"
#include "metamanager_chan.cpp"

#define CREATE_BAN 					-1
#define EXCLUDE_INCLUDE_NONE_BAN	0	
#define REMOVE_BAN 					1




static struct baninfo {
    string username;
    string reason;
} baninfo_t;

static vector<baninfo>  baninfotbl;


string generate_bandata(int banindex) {
	vector<string> retvec;
	if (banindex >= baninfotbl.size()) {
		cout << "Error: accessing banindex " << banindex << " when bansize = " << baninfotbl.size() << endl;
		return "";
	}
	retvec.push_back(baninfotbl[banindex].username);
	retvec.push_back(baninfotbl[banindex].reason);

	return joinvector("|", retvec) + "\n";
}

static int banname_to_index(string banname) {
	for (int j = 0; j < baninfotbl.size(); j++)
		if (!baninfotbl[j].username.compare(banname))
			return j;
	return -1;
}

static int is_banned(string username) {
	if (banname_to_index(username) != -1)
		return 1;
	return 0;
}

static void write_banfile(int exclude_include_mode = EXCLUDE_INCLUDE_NONE_BAN, string exclude_include_name = "", string include_reason = "") {
	ftruncate(fileno(banfile), 0);
	string printtofile = string("");
	for (int i = 0; i < baninfotbl.size(); i++) {
		if (exclude_include_mode == REMOVE_BAN && !baninfotbl[i].username.compare(exclude_include_name))
			printtofile = "";
		else
			printtofile = generate_bandata(i);
		fprintf(banfile, printtofile.c_str());
		fflush(banfile);
		
		printtofile = string("");
	}
	if (exclude_include_mode == CREATE_BAN) {
		fprintf(banfile, "%s|%s\n", exclude_include_name.c_str(), include_reason.c_str());
		fflush(banfile);
	}
}

#endif