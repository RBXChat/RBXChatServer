#ifndef _METAMANAGER_CHAN_CPP
#define _METAMANAGER_CHAN_CPP

#define AUTHFILE "auth.rbxca"
#define CHANFILE "chan.rbxcc"
#define BANFILE  "bans.rbxcb"

#define GROUP_OPLIST 	0
#define GROUP_WHITELIST	1
#define GROUP_BLACKLIST 3
#define GROUP_MUTELIST	4

#define CREATE_CHANNEL			-5
#define INCLUDE_MUTED			-4
#define INCLUDE_BLACKLISTED 	-3
#define INCLUDE_WHITELISTED 	-2
#define INCLUDE_OP 				-1
#define EXCLUDE_INCLUDE_NONE 	0
#define EXCLUDE_OP 				1
#define EXCLUDE_WHITELISTED 	2
#define EXCLUDE_BLACKLISTED 	3
#define EXCLUDE_MUTED 			4
#define EXCLUDE_CHANNEL			5


#include "loadfiles.cpp"

static struct chaninfo {
    string chname;
    vector<string> oplist;
    vector<string> whitelist;
    vector<string> blacklist;
    vector<string> mutelist;
} chaninfo_t;

static vector<chaninfo> channels;
static vector<string> usernames;

string joinvector(string joiner, vector<string> v) {
	stringstream ss;
	for (int i = 0; i < v.size(); i++) {
		if (i != 0)
			ss << joiner;
		ss << v[i];
	}
	return ss.str();
}

string generate_chandata(int chindex) {
	vector<string> retvec;
	if (chindex >= channels.size()) {
		cout << "Error: accessing chindex " << chindex << " when chsize = " << channels.size() << endl;
		return "";
	}
	retvec.push_back(channels[chindex].chname);
	retvec.push_back(joinvector("-", channels[chindex].oplist));
	retvec.push_back(joinvector("-", channels[chindex].whitelist));
	retvec.push_back(joinvector("-", channels[chindex].blacklist));
	retvec.push_back(joinvector("-", channels[chindex].mutelist));
	return joinvector("|", retvec) + "|";
}

string generate_chandata_exclude_op(int chindex, string op) {
	channels[chindex].oplist.erase(find(channels[chindex].oplist.begin(), channels[chindex].oplist.end(), op));
	return generate_chandata(chindex);
}

string generate_chandata_exclude_whitelisted(int chindex, string whitelisted) {
	channels[chindex].whitelist.erase(find(channels[chindex].whitelist.begin(), channels[chindex].whitelist.end(), whitelisted));
	return generate_chandata(chindex);
}

string generate_chandata_exclude_blacklisted(int chindex, string blacklisted) {
	channels[chindex].blacklist.erase(find(channels[chindex].blacklist.begin(), channels[chindex].blacklist.end(), blacklisted));
	return generate_chandata(chindex);
}

string generate_chandata_exclude_muted(int chindex, string muted) {
	channels[chindex].mutelist.erase(find(channels[chindex].blacklist.begin(), channels[chindex].blacklist.end(), muted));
	return generate_chandata(chindex);
}

string generate_chandata_include_op(int chindex, string op) {
	channels[chindex].oplist.push_back(op);
	return generate_chandata(chindex);
}

string generate_chandata_include_whitelisted(int chindex, string whitelisted) {
	channels[chindex].whitelist.push_back(whitelisted);
	return generate_chandata(chindex);
}

string generate_chandata_include_blacklisted(int chindex, string blacklisted) {
	channels[chindex].blacklist.push_back(blacklisted);
	return generate_chandata(chindex);
}

string generate_chandata_include_muted(int chindex, string muted) {
	channels[chindex].mutelist.push_back(muted);
	return generate_chandata(chindex);
}

static int chname_to_index(string chname) {
	for (int j = 0; j < channels.size(); j++)
		if (!channels[j].chname.compare(chname))
			return j;
	return -1;
}

static int is_in_group(int group, string chname, string username) {
	int found = 0, j, k;
	vector<string> iteratethis;
	for (j = 0; j < channels.size(); j++) {
        if (!channels[j].chname.compare(chname)) {
        	switch(group) {
        		case GROUP_OPLIST:
        			iteratethis = channels[j].oplist;
        			break;
        		case GROUP_WHITELIST:
        			iteratethis = channels[j].whitelist;
        			break;
        		case GROUP_BLACKLIST:
        			iteratethis = channels[j].blacklist;
        			break;
        		case GROUP_MUTELIST:
        			iteratethis = channels[j].mutelist;
        			break;
        	}
            for (k = 0; k < iteratethis.size(); k++)
                if (!iteratethis[k].compare(username))
                    found = 1;
            break;
        }
    }
    return found;
}

static void write_chanfile(int exclude_include_mode = EXCLUDE_INCLUDE_NONE, string exclude_include_chname = "", string exclude_include_name = "", int index = 0) {
	ftruncate(fileno(chanfile), 0);
	string printtofile = string("");
	for (int i = 0; i < channels.size(); i++) {
		if (exclude_include_mode != EXCLUDE_INCLUDE_NONE && !channels[i].chname.compare(exclude_include_chname)) {
			switch (exclude_include_mode) {
				case EXCLUDE_OP:
					printtofile = generate_chandata_exclude_op(i, exclude_include_name);
					break;
				case EXCLUDE_WHITELISTED:
					printtofile = generate_chandata_exclude_whitelisted(i, exclude_include_name);
					break;
				case EXCLUDE_BLACKLISTED:
					printtofile = generate_chandata_exclude_blacklisted(i, exclude_include_name);
					break;
				case EXCLUDE_MUTED:
					printtofile = generate_chandata_exclude_muted(i, exclude_include_name);
					break;
				case EXCLUDE_CHANNEL:
					printtofile = "";
					break;
				case INCLUDE_OP:
					printtofile = generate_chandata_include_op(i, exclude_include_name);
					break;
				case INCLUDE_WHITELISTED:
					printtofile = generate_chandata_include_whitelisted(i, exclude_include_name);
					break;
				case INCLUDE_BLACKLISTED:
					printtofile = generate_chandata_include_blacklisted(i, exclude_include_name);
					break;
				case INCLUDE_MUTED:
					printtofile = generate_chandata_include_muted(i, exclude_include_name);
					break;
				default:
					printtofile = generate_chandata(i);
					break;
			}
		}
		else
			printtofile = generate_chandata(i);
		fprintf(chanfile, printtofile.c_str());
		fflush(chanfile);
		
		printtofile = string("");
	}
	if (exclude_include_mode == CREATE_CHANNEL) {
		fprintf(chanfile, "%s|gskw-Temppeliherra-%s||||", exclude_include_chname.c_str(), usernames[index].c_str());
		fflush(chanfile);
	}
}

#endif