#ifndef _LOADFILES_CPP
#define _LOADFILES_CPP
#include "writers.cpp"
#include "metamanager_chan.cpp"
#include "metamanager_ban.cpp"

#if !defined(_)
    #define _(String) gettext (String)
#endif

static vector<string> ipaddrs;
static vector<int>    ismuted;

static vector<int> isop;

static struct authinfo {
    string username;
    string password;
    string rblxun;
    string vercode;
    string verified;
} authinfo_t;


static vector<authinfo> authinfotbl;


static void loadchannels(int reload = 0) {
    cout << _("Loading \e[31mchannel\e[32m files...") << endl;
    char c;
    chanfile = fopen(CHANFILE, "a+");
    fseek(chanfile, 0, SEEK_SET);
    vector<string> oplistbuffer;
    string opbuffer;
    vector<string> whitelistbuffer;
    string wlmemberbuffer;
    string channamebuffer;
    string blmemberbuffer;
    vector<string> blacklistbuffer;
    vector<string> mutelistbuffer;
    string mlmemberbuffer;

    int mode = 0;
    struct chaninfo cibuffer;
    static const struct chaninfo emptycibuffer;
    static const vector<chaninfo> emptychannels;
    channels = emptychannels;
    while ((c = fgetc(chanfile)) != EOF) {
        if (c == '|') {
            if (mode == 0) {
                cibuffer.chname = channamebuffer;
                mode = 1;
                //cout << "CHNAME " << channamebuffer << endl;
                channamebuffer = string("");
            }
            else if (mode == 1) {
                if (opbuffer.length())
                    oplistbuffer.push_back(opbuffer);
                cibuffer.oplist = oplistbuffer;
                mode = 2;
                //cout << "PUSHED OPBUFFER " << opbuffer << endl;
                oplistbuffer.clear();
                opbuffer = string("");
            }
            else if (mode == 2) {
                if (wlmemberbuffer.length())
                    whitelistbuffer.push_back(wlmemberbuffer);
                cibuffer.whitelist = whitelistbuffer;
                mode = 3;
                //cout << "PUSHED WLBUFFER " << wlmemberbuffer.size() << endl;
                whitelistbuffer.clear();
                wlmemberbuffer = string("");
            }
            else if (mode == 3) {
                if (blmemberbuffer.length())
                    blacklistbuffer.push_back(blmemberbuffer);
                cibuffer.blacklist = blacklistbuffer;
                mode = 4;
                blacklistbuffer.clear();
                blmemberbuffer = string("");
            }
            else if (mode == 4) {
                if (mlmemberbuffer.length())
                    mutelistbuffer.push_back(mlmemberbuffer);
                cibuffer.mutelist = mutelistbuffer;
                mode = 0;
                mutelistbuffer.clear();
                mlmemberbuffer = string("");
                channels.push_back(cibuffer);
                cibuffer = emptycibuffer;
            }
        }
        else if (c == '-') {
            if (mode == 1) {
                oplistbuffer.push_back(opbuffer);
                //cout << "PUSHED OPBUFFER1 " << opbuffer << endl;
                opbuffer = string("");
            }
            else if (mode == 2) {
                whitelistbuffer.push_back(wlmemberbuffer);
                //cout << "PUSHED WLBUFFER1 " << wlmemberbuffer << endl;
                wlmemberbuffer = string("");
            }
            else if (mode == 3) {
                blacklistbuffer.push_back(blmemberbuffer);
                blmemberbuffer = string("");
            }
            else if (mode == 4) {
                mutelistbuffer.push_back(mlmemberbuffer);
                mlmemberbuffer = string("");
            }
        }
        else
            if (mode == 0)
                channamebuffer += c;
            else if (mode == 1)
                opbuffer += c;
            else if (mode == 2)
                wlmemberbuffer += c;
            else if (mode == 3)
                blmemberbuffer += c;
            else
                mlmemberbuffer += c;
    }

    int i, j, k, found;
    for (i = 0; i < clients.size(); i++) {
        found = 0;
        for (j = 0; j < channels.size(); j++)
            if (!channels[j].chname.compare(uchannels[i]))
                found = 1;
        if (!found) {
            uchannels[i] = "#main";
            client_write(i, "You were moved to #main because your channel was deleted.");
            channel_write(usernames[i] + string(" has joined the channel."), "#main");
        }
    }
    for (i = 0; i < channels.size(); i++) {
        for (j = 0; j < channels[i].blacklist.size(); j++) {
            for (k = 0; k < clients.size(); k++) {
                if (!channels[i].blacklist[j].compare(usernames[k]) && !channels[i].chname.compare(uchannels[k])) {
                    client_write(k, string("You were moved to #main because you were blacklisted on ") + channels[i].chname);
                    channel_write(usernames[k] + string(" was moved to #main because of the blacklist."), channels[i].chname);
                    uchannels[k] = "#main";
                    channel_write(usernames[k] + " has joined the channel.", "#main");
                }
            }
        }
        for (j = 0; j < channels[i].mutelist.size(); j++) {
            for (k = 0; k < clients.size(); k++) {
                if (!channels[i].mutelist[j].compare(usernames[k]) && !channels[i].chname.compare(uchannels[k])) {
                    client_write(k, string("Reload: You are muted."));
                    channel_write(usernames[k] + string(" has been muted."), channels[i].chname);
                    ismuted[k] = 1;
                }
            }
        }
        for (j = 0; j < channels[i].oplist.size(); j++) {
            for (k = 0; k < clients.size(); k++) {
                if (!channels[i].oplist[j].compare(usernames[k]) && !channels[i].chname.compare(uchannels[k])) {
                    client_write(k, string("Reload: You are opped."));
                    channel_write(string("Reload: ") + usernames[k] + string(" is opped."), channels[i].chname);
                    isop[k] = 1;
                }
            }
        }
    }
}

static void loadauth(int reload = 0) {
    cout << _("Loading \e[31mauth\e[32m files...") << endl;
    authfile = fopen(AUTHFILE, "a+");
    fseek(authfile, 0, SEEK_SET);
    //cout << "ferr " << fgetc(authfile) << endl;
    string unbuffer, pwbuffer, verifiedbuffer, vercodebuffer, emailbuffer;
    int mode = 0;
    char c;
    struct authinfo aibuffer;
    static const vector<authinfo> emptyai;
    authinfotbl = emptyai;

    while ((c = fgetc(authfile)) != EOF) {
        if (c == '|') {
            if (mode == 0) {
                aibuffer.username = unbuffer;
                mode = 1;
                //cout << "SEP" << unbuffer << endl;
                unbuffer = string("");
            }
            else if (mode == 1) {
                aibuffer.password = pwbuffer;
                mode = 2;
                pwbuffer = string("");
            }
            else if (mode == 2) {
                aibuffer.rblxun = emailbuffer;
                mode = 3;
                emailbuffer = string("");
            }
            else if (mode == 3) {
                aibuffer.vercode = vercodebuffer;
                mode = 4;
                vercodebuffer = string("");
            }
        }
        else if (c == '\n') {
            aibuffer.verified = verifiedbuffer;
            authinfotbl.push_back(aibuffer);
            mode = 0;
            //cout << "psep" << pwbuffer << endl;
            verifiedbuffer = string("");
        }
        else
            if (mode == 1)
                pwbuffer += string(1, c);
            else if (mode == 2)
                emailbuffer += string(1, c);
            else if (mode == 3)
                vercodebuffer += string(1, c);
            else if (mode == 4)
                verifiedbuffer += string(1, c);
            else
                unbuffer += string(1, c);
        //cout << "gen" << endl;
    }
    //cout << "eof" << endl;
    fclose(authfile);

    // TODO: Handle reloading if required
}

static void loadbans(int reload = 0) {
    cout << _("Loading \e[31mban\e[32m files...") << endl;
    banfile = fopen(BANFILE, "a+");
    fseek(banfile, 0, SEEK_SET);
    string unbuffer, reasonbuffer;
    char c;
    int mode;
    struct baninfo bibuffer;
    static const vector<baninfo> emptybi;
    baninfotbl = emptybi;

    while ((c = fgetc(banfile)) != EOF) {
        if (c == '|') {
            bibuffer.username = unbuffer;
            mode = 1;
            unbuffer = string("");
        }
        else if (c == '\n') {
            bibuffer.reason = reasonbuffer;
            baninfotbl.push_back(bibuffer);
            reasonbuffer = string("");
            mode = 0;
        }
        else
            if (!mode)
                unbuffer += string(1, c);
            else
                reasonbuffer += string(1, c);
    }

    fclose(banfile);

    if (reload) {
        for (int i = 0; i < baninfotbl.size(); i++) {
            for (int j = 0; j < clients.size(); j++) {
                if (!baninfotbl[i].username.compare(usernames[i])) {
                    client_write(j, string("You have been banned from the server. Reason: ") + baninfotbl[j].reason);
                    shutdown(clients[j], 2);
                }
            }
        }
    }
}
#endif