#ifndef _WRITERS_CPP
#define _WRITERS_CPP

static int numclients = 0;
static vector<string> uchannels;
static FILE* authfile, *chanfile, *banfile;

static vector<int> clients;

static void client_write(int client, string msg) {
    stringstream ss;
    ss << clients[client];
    system((string(PHPPATH) + " -f parsemessage.php -- '" + msg + "' \"" + ss.str() + "\"").c_str());
}

static void channel_write(string msg, string channel) {
    int i;
    string writeclients = "";
    for (i = 0; i < numclients; i++) {
        if (!uchannels[i].compare(channel)) {
            stringstream ss;
            ss << clients[i];
            writeclients += ss.str() + string(",");
            //cout << string("appended") << clients[i] << endl;
        }
    }
    system((string(PHPPATH) + string(" -f parsemessage.php -- '") + msg + string("' \"") + writeclients + string("\"")).c_str());
}

static void global_write(string msg) {
    int i;
    string writeclients;
    for (i = 0; i < numclients; i++) {
        stringstream ss;
        ss << clients[i];
        writeclients += ss.str() + string(",");
    }
    system((string(PHPPATH) + string(" -f parsemessage.php -- '") + msg + string("' \"") + writeclients + string("\"")).c_str());
}

#endif