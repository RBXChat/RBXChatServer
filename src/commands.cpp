#ifndef _COMMANDS_CPP
#define _COMMANDS_CPP
#include "loadfiles.cpp"


#define CMD_JOINED              '\x01'
#define CMD_LEFT                '\x02'
#define CMD_REGISTERED          '\x03'
#define CMD_REJECTED            '\x04'
#define CMD_CHANGE_CHANNEL      '\x05'
#define CMD_BROADCAST           '\x06'
#define CMD_PRIVATE_MESSAGE     '\x07'
#define CMD_KICK                '\x08'
#define CMD_BAN                 '\x09'
#define CMD_UNBAN               '\x0A'
#define CMD_OP                  '\x0B'
#define CMD_DEOP                '\x0C'
#define CMD_WHITELIST           '\x0D'
#define CMD_DEWHITELIST         '\x0E'
#define CMD_GET_IP_ADDRESS      '\x0F'
#define CMD_FIND_USER           '\x10'
#define CMD_CHAT_AS             '\x11'
#define CMD_COMMAND_AS          '\x12'
#define CMD_RELOAD              '\x13'
#define CMD_FIND_ROBLOX_ACCOUNT '\x14'
#define CMD_SHUTDOWN            '\x15'
#define CMD_CREATE_CHANNEL      '\x16'
#define CMD_DELETE_CHANNEL      '\x17'
#define CMD_LIST_CHANNELS       '\x18'
#define CMD_ME                  '\x19'
#define CMD_MUTE                '\x1A'
#define CMD_UNMUTE              '\x1B'
#define CMD_BLACKLIST           '\x1C'
#define CMD_DEBLACKLIST         '\x1D'
#define CMD_GET_BLACKLIST       '\x1E'
#define CMD_GET_WHITELIST       '\x1F'
#define CMD_GET_OPLIST          '\x20'
#define CMD_GET_MUTELIST        '\x21'
#define CMD_GET_ONLINE_LIST     '\x22'
#define CMD_WHERE_AM_I          '\x23'

static void cleanup(int voidthis = 0) {
    char outfilename[50];
    char filename[50];
    char connfilename[50];
    for (int i = 0; i < sizeof(clients) / sizeof(clients[0]); i++) {
        close(clients[i]);
        sprintf(connfilename, "buffers/connhandler_%d", clients[i]);
        sprintf(filename, "buffers/buffer_%d", clients[i]);
        sprintf(outfilename, "buffers/buffer_out_%d", clients[i]);
        unlink(connfilename);
        unlink(filename);
        unlink(outfilename);
    }
    cout << _("Cleanup done!") << endl;
    exit(0);
}


static void evaluate_command(FILE* connstream, string command) {
    if (!command.find(".join #")) {
        cout << _("Joining channel ") << command.substr(strlen(".join #")) << endl;
        fprintf(connstream, "%c%s", CMD_CHANGE_CHANNEL, command.substr(strlen(".join #")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".broadcast ")) {
        cout << _("Broadcast attempt ") << command.substr(strlen(".broadcast ")) << endl;
        fprintf(connstream, "%c%s", CMD_BROADCAST, command.substr(strlen(".broadcast ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".msg ")) {
        cout << _("Messaging ") << command.substr(strlen(".msg ")) << endl;
        fprintf(connstream, "%c%s", CMD_PRIVATE_MESSAGE, command.substr(strlen(".msg ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".kick ")) {
        cout << _("Kicking ") << command.substr(strlen(".kick ")) << endl;
        fprintf(connstream, "%c%s", CMD_KICK, command.substr(strlen(".kick ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".ban ")) {
        cout << _("Banning ") << command.substr(strlen(".ban ")) << endl;
        fprintf(connstream, "%c%s", CMD_BAN, command.substr(strlen(".ban ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".unban ")) {
        cout << _("Unbanning ") << command.substr(strlen(".unban ")) << endl;
        fprintf(connstream, "%c%s", CMD_UNBAN, command.substr(strlen(".unban ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".op ")) {
        cout << _("Opping ") << command.substr(strlen(".op ")) << endl;
        fprintf(connstream, "%c%s", CMD_OP, command.substr(strlen(".op ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".deop ")) {
        cout << _("Deopping ") << command.substr(strlen(".deop ")) << endl;
        fprintf(connstream, "%c%s", CMD_DEOP, command.substr(strlen(".deop ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".whitelist ")) {
        cout << _("Whitelisting ") << command.substr(strlen(".whitelist ")) << endl;
        fprintf(connstream, "%c%s", CMD_WHITELIST, command.substr(strlen(".whitelist ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".dewhitelist ")) {
        cout << _("Dewhitelisting ") << command.substr(strlen(".dewhitelist ")) << endl;
        fprintf(connstream, "%c%s", CMD_DEWHITELIST, command.substr(strlen(".dewhitelist ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".getipaddr ")) {
        cout << _("Getting IP address ") << command.substr(strlen(".getipaddr ")) << endl;
        fprintf(connstream, "%c%s", CMD_GET_IP_ADDRESS, command.substr(strlen(".getipaddr ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".finduser ")) {
        cout << _("Finding user ") << command.substr(strlen(".finduser ")) << endl;
        fprintf(connstream, "%c%s", CMD_FIND_USER, command.substr(strlen(".finduser ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".chatas ")) {
        cout << _("Chatting as ") << command.substr(strlen(".chatas ")) << endl;
        fprintf(connstream, "%c%s", CMD_CHAT_AS, command.substr(strlen(".chatas ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".commandas ")) {
        cout << _("Commanding as ") << command.substr(strlen(".commandas ")) << endl;
        fprintf(connstream, "%c%s", CMD_COMMAND_AS, command.substr(strlen(".commandas ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".reload")) {
        cout << _("Reloading");
        fprintf(connstream, "%c", CMD_RELOAD);
        fflush(connstream);
    }

    else if (!command.find(".robloxacc ")) {
        cout << _("Finding ROBLOX account ") << command.substr(strlen(".robloxacc ")) << endl;
        fprintf(connstream, "%c%s", CMD_FIND_ROBLOX_ACCOUNT, command.substr(strlen(".robloxacc ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".shutdown ")) {
        cout << _("Shutting down ") << command.substr(strlen(".shutdown ")) << endl;
        fprintf(connstream, "%c%s", CMD_SHUTDOWN, command.substr(strlen(".shutdown ")).c_str());
        fflush(connstream);
    }

    else if (!command.find(".createchannel ")) {
        cout << _("Creating channel ") << command.substr(strlen(".createchannel ")) << endl;
        fprintf(connstream, "%c%s", CMD_CREATE_CHANNEL, command.substr(strlen(".createchannel ")).c_str());
        fflush(connstream);
    }
    else if (!command.find(".deletechannel ")) {
        cout << _("Deleting channel ") << command.substr(strlen(".deletechannel ")) << endl;
        fprintf(connstream, "%c%s", CMD_DELETE_CHANNEL, command.substr(strlen(".deletechannel ")).c_str());
        fflush(connstream);
    }
    else if (!command.find(".listchannels")) {
        cout << _("Listing channels ") << command.substr(strlen(".listchannels")) << endl;
        fprintf(connstream, "%c%s", CMD_LIST_CHANNELS, command.substr(strlen(".listchannels")).c_str());
        fflush(connstream);
    }
    else if (!command.find(".me ")) {
        cout << _("Sending me ") << command.substr(strlen(".me ")) << endl;
        fprintf(connstream, "%c%s", CMD_ME, command.substr(strlen(".me ")).c_str());
        fflush(connstream);
    }
    else if (!command.find(".mute ")) {
        cout << _("Muting ") << command.substr(strlen(".mute ")) << endl;
        fprintf(connstream, "%c%s", CMD_MUTE, command.substr(strlen(".mute ")).c_str());
        fflush(connstream);
    }
    else if (!command.find(".unmute ")) {
        cout << _("Unmuting ") << command.substr(strlen(".unmute ")) << endl;
        fprintf(connstream, "%c%s", CMD_UNMUTE, command.substr(strlen(".unmute ")).c_str());
        fflush(connstream);
    }
    else if (!command.find(".blacklist ")) {
        cout << _("Blacklisting ") << command.substr(strlen(".blacklist ")) << endl;
        fprintf(connstream, "%c%s", CMD_BLACKLIST, command.substr(strlen(".blacklist ")).c_str());
        fflush(connstream);
    }
    else if (!command.find(".deblacklist ")) {
        cout << _("Deblacklisting ") << command.substr(strlen(".deblacklist ")) << endl;
        fprintf(connstream, "%c%s", CMD_DEBLACKLIST, command.substr(strlen(".deblacklist ")).c_str());
        fflush(connstream);
    }
    else if (!command.find(".getblacklist")) {
        cout << _("Getting blacklist") << endl;
        fprintf(connstream, "%c", CMD_GET_BLACKLIST);
        fflush(connstream);
    }
    else if (!command.find(".getwhitelist")) {
        cout << _("Getting whitelist") << endl;
        fprintf(connstream, "%c", CMD_GET_WHITELIST);
        fflush(connstream);
    }
    else if (!command.find(".getoplist")) {
        cout << _("Getting OP list") << endl;
        fprintf(connstream, "%c", CMD_GET_OPLIST);
        fflush(connstream);
    }
    else if (!command.find(".getmutelist")) {
        cout << _("Getting mute list") << endl;
        fprintf(connstream, "%c", CMD_GET_MUTELIST);
        fflush(connstream);
    }
    else if (!command.find(".getonlist")) {
        cout << _("Getting online list") << endl;
        fprintf(connstream, "%c", CMD_GET_ONLINE_LIST);
        fflush(connstream);
    }
    else if (!command.find(".whereami")) {
        cout << _("Getting current channel") << endl;
        fprintf(connstream, "%c", CMD_WHERE_AM_I);
        fflush(connstream);
    }

    else if (!command.find("."))
        cout <<  _("issued an unknown command ") << command << endl;
}




static void disconnect(int i, char c, FILE* connstream, const char* connfilename) {
    cout << _("Disconnect: ") << i << endl;
    --numclients;
    shutdown(clients[i], 2);
    clients.erase(clients.begin() + i);
    if (c != CMD_REJECTED) { // If the user was rejected, he was not added to the username list
    	global_write(_("&lt;<b>SYSTEM</b>&gt; <i>") + usernames[i] + _("</i> has left."));
    	usernames.erase(usernames.begin() + i);
    	uchannels.erase(uchannels.begin() + i);
        ipaddrs.erase(ipaddrs.begin() + i);
        ismuted.erase(ismuted.begin() + i);
        isop.erase(isop.begin() + i);
    }
    fclose(connstream);
    unlink(connfilename);
}

static void join(int i, char c, FILE* connstream) {
    char cmd = c;
    int k;
    //cout << "Something" << endl;
    string buf;
    fseek(connstream, 1, SEEK_SET);
    while ((c = fgetc(connstream)) != EOF)
        buf += string(1, c);
    //buf[buf.length()] = '\0';
    //cout << "PUSHING BACK" << endl;
    usernames.push_back(buf);
    uchannels.push_back("#main");
    ismuted.push_back(0);
    isop.push_back(0);
    cout << _("Join: ") << buf << endl;
    channel_write(_("&lt;<b>SYSTEM</b>&gt; <i>") + buf + _("</i> has joined the channel!"), "#main");
    if (cmd == CMD_REGISTERED) {
        ftruncate(fileno(connstream), 0);
        fclose(connstream);
        loadauth();
    }
    for (k = 0; k < channels[0].oplist.size(); k++)
        if (!channels[0].oplist[k].compare(usernames[i]))
            isop[i] = 1;
    for (k = 0; k < channels[0].mutelist.size(); k++)
        if (!channels[0].mutelist[k].compare(usernames[i]))
            ismuted[i] = 1;
}

static int change_channel(int i, FILE* connstream) {
    int j, k, chfound;
    cout << _("Got \e[34mchannel change\e[32m command ") << endl;
    string buf;
    char c;
    while ((c = fgetc(connstream)) != EOF)
        buf += string(1, c);

    // ACCESS CHECK
    chfound = 0;
    for (j = 0; j < channels.size(); j++) {
        if (!channels[j].chname.compare("#" + buf)) {
            chfound = 1;
            if (channels[j].whitelist.size()) {
                //cout << "whitelist test" << channels[j].whitelist.size() << endl;
                int isallowed = 0;
                for (k = 0; k < channels[j].whitelist.size(); k++) {
                    //cout << "Testing for " << channels[j].whitelist[k] << endl << flush;
                    if (!channels[j].whitelist[k].compare(usernames[i]))
                        isallowed = 1;
                }
                if (!isallowed) {
                    ftruncate(fileno(connstream), 0);
                    client_write(i, _("&lt;<b>SYSTEM</b>&gt; You are not whitelisted on channel #") + buf + "!\n");
                    return 1;
                }
            }
            else if (channels[j].blacklist.size()) {
                for (k = 0; k < channels[j].blacklist.size(); k++) {
                    if (!channels[j].blacklist[k].compare(usernames[i])) {
                        ftruncate(fileno(connstream), 0);
                        client_write(i, _("&lt;<b>SYSTEM</b>&gt; You are blacklisted on channel #") + buf + "!\n");
                        return 1;
                    }
                }
            }
            //cout << "He is whitelisted" << endl;
            isop[i] = 0;
            ismuted[i] = 0;
            for (k = 0; k < channels[j].oplist.size(); k++)
                if (!channels[j].oplist[k].compare(usernames[i]))
                    isop[i] = 1;
            for (k = 0; k < channels[j].mutelist.size(); k++)
                if (!channels[j].mutelist[k].compare(usernames[i]))
                    ismuted[i] = 1;
            if (isop[i])
                cout << _("He is opped on the channel") << endl;
        }
    }

    if (chfound) {
        channel_write(_("&lt;<b>SYSTEM</b>&gt; <i>") + usernames[i] + _("</i> has left the channel!<br />"), uchannels[i]);
        uchannels[i] = "#" + buf;
        cout << "New channel " << uchannels[i] << ": " << usernames[i] << endl;
        client_write(i, _("&lt;<b>SYSTEM</b>&gt; You have joined #") + buf + "<br />");
        channel_write(_("&lt;<b>SYSTEM</b>&gt; <i>") + usernames[i] + _("</i> has joined the channel!<br />"), "#" + buf);
    }
    else
        client_write(i, _("&lt;<b>SYSTEM</b>&gt; #") + buf + _(" does not exist!"));
    return 0;
}

static void broadcast(int i, FILE* connstream) {
    cout << _("Got \e[34mbroadcast\e[32m command ") << endl;
    string buf;
    char c;
    while ((c = fgetc(connstream)) != EOF)
        buf += string(1, c);
    cout << _("Trying to broadcast ") << isop[i] << endl;
    if (isop[i])
        //cout << "GLOBWRITE CALL" << endl;
        global_write(string(_("[<b>BROADCAST</b>] ")) + buf);
    else 
        //cout << "CWRITE CALL" << endl;
        client_write(i, _("You are not opped on the channel!"));
}

static void pm(int i, FILE* connstream) {
    cout << _("Got \e[34mmessage\e[32m command ") << endl;
    string buf, msgbuf;
    int mode = 0;
    char c;
    while ((c = fgetc(connstream)) != EOF) {
        if (c == ' ' && mode == 0)
            mode = 1;
        else
            if (mode == 0)
                buf += string(1, c);
            else
                msgbuf += string(1, c);
    }
    vector<string>::iterator userpos;
    userpos = find(usernames.begin(), usernames.end(), buf);
    if (userpos == usernames.end())
        client_write(i, string(_("The user ")) + buf + string(_(" was not found.")));
    else {
        client_write((int)distance(usernames.begin(), userpos), string("[") + usernames[i] + string(_(" -> you] ")) + msgbuf);
        client_write(i, string(_("[you -> ")) + buf + string("] ") + msgbuf);
    }
}

static void kick(int i, FILE* connstream) {

    cout << _("Got \e[34mkick\e[32m command") << endl;
    if (isop[i]) {
        string buf, msgbuf;
        int mode = 0;
        char c;
        while ((c = fgetc(connstream)) != EOF) {
            if (c == ' ' && mode == 0)
                mode = 1;
            else
                if (mode == 0)
                    buf += string(1, c);
                else
                    msgbuf += string(1, c);
        }
        vector<string>::iterator userpos;
        userpos = find(usernames.begin(), usernames.end(), buf);
        if (userpos == usernames.end())
            client_write(i, string(_("The user ")) + buf + string(_(" was not found.")));
        else {
            client_write((int)distance(usernames.begin(), userpos), string(_("You have been kicked from the server. Reason: ")) + msgbuf);
            global_write(usernames[i] + string(_(" has kicked the user ")) + buf + string(_(" from the server. Reason: ")) + msgbuf);
            shutdown(clients[(int)distance(usernames.begin(), userpos)], 2);
        }
    }
    else 
        client_write(i, _("You are not opped on the channel!"));
}

static void ban(int i, FILE* connstream) {
    cout << _("Got \e[34mban\e[32m command") << endl;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        string buf, msgbuf;
        int mode = 0;
        char c;
        while ((c = fgetc(connstream)) != EOF) {
            if (c == ' ' && mode == 0)
                mode = 1;
            else
                if (mode == 0)
                    buf += string(1, c);
                else
                    msgbuf += string(1, c);
        }
        vector<string>::iterator userpos;
        userpos = find(usernames.begin(), usernames.end(), buf);
        if (userpos == usernames.end())
            client_write(i, string(_("The user ")) + buf + string(_(" was not found.")));
        else {
            client_write((int)distance(usernames.begin(), userpos), string(_("You have been banned from the server. Reason: ")) + msgbuf);
            global_write(usernames[i] + string(_(" has banned the user ")) + buf + string(_(" from the server. Reason: ")) + msgbuf);
            shutdown(clients[(int)distance(usernames.begin(), userpos)], 2);
            write_banfile(CREATE_BAN, buf, msgbuf);
            ftruncate(fileno(connstream), 0);
            loadbans(1);
        }
    }
}

static void unban(int i, FILE* connstream) {
    cout << _("Got \e[34munban\e[32m command") << endl;
    int success = 0;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        string buf;
        char c;
        int j;
        while ((c = fgetc(connstream)) != EOF)
            buf += string(1, c);
        
        if (!is_banned(buf))
            client_write(i, _("User is not banned!"));
        else {
            write_banfile(REMOVE_BAN, buf);
            client_write(i, string(_("Unbanned ")) + buf);
            fflush(banfile);
            fclose(banfile);
            ftruncate(fileno(connstream), 0);
            loadbans(1);
        }
    }
}

static int op(int i, FILE* connstream) {
    cout << _("Got \e[34mop\e[32m command") << endl;
    string buf;
    int j, k, l;
    char c;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        buf = string("");
        while ((c = fgetc(connstream)) != EOF)
            buf += string(1, c);

        int chindex = chname_to_index(uchannels[i]);
        if (is_in_group(GROUP_OPLIST, uchannels[i], buf))
            client_write(i, _("The user is already opped on the channel!"));
        else {
            write_chanfile(INCLUDE_OP, uchannels[i], buf);
            ftruncate(fileno(connstream), 0);
            client_write(i, string(_("Succesfully opped ")) + buf);
            loadchannels(1);
        }
        return 0;
    }
    return 0;
}

static int deop(int i, FILE* connstream) {
    cout << _("Got \e[34mdeop\e[32m command") << endl;
    string buf;
    int j, k, l;
    char c;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        buf = string("");
        while ((c = fgetc(connstream)) != EOF)
            buf += string(1, c);

        int chindex = chname_to_index(uchannels[i]);
        if (!is_in_group(GROUP_OPLIST, uchannels[i], buf))
            client_write(i, _("The user is not opped on the channel!"));
        else {
            write_chanfile(EXCLUDE_OP, uchannels[i], buf);
            ftruncate(fileno(connstream), 0);
            client_write(i, string(_("Succesfully deopped ")) + buf);
            loadchannels(1);
        }
        return 0;
    }
}

static int whitelist(int i, FILE* connstream) {
    cout << _("Got \e[34mop\e[32m command") << endl;
    string buf;
    int j, k, l;
    char c;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        buf = string("");
        while ((c = fgetc(connstream)) != EOF)
            buf += string(1, c);
        chanfile = fopen(CHANFILE, "a+");

        int chindex = chname_to_index(uchannels[i]);
        if (is_in_group(GROUP_WHITELIST, uchannels[i], buf))
            client_write(i, _("The user is already whitelisted on the channel!"));
        else {
            write_chanfile(INCLUDE_WHITELISTED, uchannels[i], buf);
            ftruncate(fileno(connstream), 0);
            client_write(i, string(_("Succesfully whitelisted ")) + buf);
            loadchannels(1);
        }
        return 0;
    }
    return 0;
}

static int dewhitelist(int i, FILE* connstream) {
    cout << _("Got \e[34mdewhitelist\e[32m command") << endl;
    string buf;
    int j, k, l;
    char c;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        buf = string("");
        while ((c = fgetc(connstream)) != EOF)
            buf += string(1, c);

        int chindex = chname_to_index(uchannels[i]);
        if (!is_in_group(GROUP_WHITELIST, uchannels[i], buf))
            client_write(i, _("The user is not whitelisted on the channel!"));
        else {
            write_chanfile(EXCLUDE_WHITELISTED, uchannels[i], buf);
            ftruncate(fileno(connstream), 0);
            client_write(i, string(_("Succesfully dewhitelisted ")) + buf);
            loadchannels(1);
        }
        return 0;
    }
}

static void getipaddr(int i, FILE* connstream) {
    cout << _("got \e[34mget ip address\e[32m command") << endl;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        string buf;
        char c;
        while ((c = fgetc(connstream)) != EOF)
            buf += string(1, c);
        vector<string>::iterator userpos;
        userpos = find(usernames.begin(), usernames.end(), buf);
        if (userpos == usernames.end())
            client_write(i, string(_("The user ")) + buf + string(_(" was not found.")));
        else {
            client_write(i, string(_("The IP address for username ")) + buf + string(_(" is ")) + string(ipaddrs[distance(usernames.begin(), userpos)]));
            stringstream ss;
            ss << clients[distance(usernames.begin(), userpos)];
            client_write(i, string(_("The sockfd for username ")) + buf + string(_(" is ")) + ss.str());
        }
    }
}

static void finduser(int i, FILE* connstream) {
    cout << _("got \e[34mfind user\e[32m command") << endl;
    string buf;
    char c;
    while ((c = fgetc(connstream)) != EOF)
        buf += string(1, c);
    vector<string>::iterator userpos;
    userpos = find(usernames.begin(), usernames.end(), buf);
    if (userpos == usernames.end())
        client_write(i, string(_("The user ")) + buf + string(_(" was not found.")));
    else
        client_write(i, string(_("The user ")) + buf + string(_(" is chatting on ")) + uchannels[distance(usernames.begin(), userpos)]);
}

static void chatas(int i, FILE* connstream) {
    cout << _("got \e[34mchat as\e[32m command") << endl;
    string buf, msgbuf;
    int mode = 0;
    char c;
    while ((c = fgetc(connstream)) != EOF) {
        if (c == ' ' && mode == 0)
            mode = 1;
        else
            if (mode == 0)
                buf += string(1, c);
            else
                msgbuf += string(1, c);
    }
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        vector<string>::iterator userpos;
        userpos = find(usernames.begin(), usernames.end(), buf);
        if (userpos == usernames.end())
            client_write(i, string(_("The user ")) + buf + string(_(" was not found.")));
        else {
            client_write(i, string(_("Chatted ")) + msgbuf + string(_(" as ")) + buf);
            channel_write(string("&lt;") + buf + string("&gt; ") + msgbuf, uchannels[distance(usernames.begin(), userpos)]);
        }
    }
}

static void commandas(int i, FILE* connstream) {
    cout << _("got \e[34mcommand as\e[32m command") << endl;
    string buf;
    char c;
    while ((c = fgetc(connstream)) != EOF)
        buf += string(1, c);
    evaluate_command(connstream, buf);
}

static void reload(int i, FILE* connstream) {
    cout << _("got \e[34mreload\e[32m command") << endl;
    client_write(i, "Reloading...");
    ftruncate(fileno(connstream), 0);
    loadauth(1);
    loadbans(1);
    loadchannels(1);
}

static void findrobloxaccount(int i, FILE* connstream) {
    cout << _("got \e[34mfind roblox account\e[32m command") << endl;
    string buf;
    char c;
    int j;
    while ((c = fgetc(connstream)) != EOF)
        buf += string(1, c);
    vector<string>::iterator userpos;
    userpos = find(usernames.begin(), usernames.end(), buf);
    if (userpos == usernames.end())
        client_write(i, string(_("The user ")) + buf + string(_(" was not found.")));
    else {
        int foundindex = -1;
        for (j = 0; j < authinfotbl.size(); j++)
            if (!authinfotbl[j].username.compare(buf))
                foundindex = j;
        if (foundindex == -1)
            client_write(i, _("Something went wrong!"));
        else
            if (!authinfotbl[foundindex].verified.compare("false"))
                client_write(i, buf + string(_(" has not verified their ROBLOX account yet. He claims it to be ")) + authinfotbl[foundindex].rblxun + string(_(" though.")));
            else
                client_write(i, buf + string(_("'s ROBLOX account is ")) + authinfotbl[foundindex].rblxun);
    }
}

static void shutdown(int i, FILE* connstream) {
    cout << _("got \e[34mshutdown\e[32m command") << endl;
    string buf;
    char c;
    while ((c = fgetc(connstream)) != EOF)
        buf += string(1, c);
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        global_write(usernames[i] + string(_(" is shutting down the server: ")) + buf);
        sleep(5);
        for (i = 0; i < clients.size(); i++)
            shutdown(clients[i], 2);
        cleanup();
        exit(0);
    }
}

static void createchannel(int i, FILE* connstream) {
    cout << _("got \e[34mcreate channel\e[32m command") << endl;
    string buf;
    char c;
    while ((c = fgetc(connstream)) != EOF)
        buf += string(1, c);
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        write_chanfile(CREATE_CHANNEL, buf, "", i);
        loadchannels(1);
    }
}

static void deletechannel(int i, FILE* connstream) {
    cout << _("got \e[34mdelete channel\e[32m command") << endl;
    string buf;
    char c;
    while ((c = fgetc(connstream)) != EOF)
        buf += string(1, c);
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        write_chanfile(EXCLUDE_CHANNEL, buf);
        client_write(i, string(_("Succesfully deleted channel ")) + buf);
        loadchannels(1);
    }
}

static void listchannels(int i, FILE* connstream) {
    cout << _("got \e[34mchannel list\e[32m command") << endl;
    string cwbuffer = string(""), currchbuffer = string("");
    for (int j = 0; j < channels.size(); j++) {
        currchbuffer = channels[j].chname;
        if (is_in_group(GROUP_OPLIST, channels[j].chname, usernames[i]))
            currchbuffer = string("<b>") + currchbuffer + string("</b>");
        if (is_in_group(GROUP_WHITELIST, channels[j].chname, usernames[i]))
            currchbuffer = string("<u>") + currchbuffer + string("</u>");
        else if (channels[j].whitelist.size())
            currchbuffer = string("<s>") + currchbuffer + string("</s>");
        if (is_in_group(GROUP_BLACKLIST, channels[j].chname, usernames[i]))
            currchbuffer = string("<s><font color='red'>") + currchbuffer + ("</font></s>");
        if (is_in_group(GROUP_MUTELIST, channels[j].chname, usernames[i]))
            currchbuffer = string("<font color='red'>") + currchbuffer + string("</font>");
        cwbuffer += currchbuffer + string(",");
        currchbuffer = string("");
    }
    cwbuffer = cwbuffer.substr(0, cwbuffer.length() - 1);
    cout << _("Going to write ") << cwbuffer << endl;
    client_write(i, cwbuffer);
    client_write(i, _("Legend: <b>bold</b> = opped, <u>underlined</u> = whitelisted, <s>striked</s> = not whitelisted, <s><font color='red'>striked red</s> = blacklisted, <font color='red'>red</font> = muted"));
}

static void me(int i, FILE* connstream) {
    cout << _("got \e[34mme\e[32m command") << endl;
    string buf;
    char c;
    while ((c = fgetc(connstream)) != EOF)
        buf += string(1, c);
    channel_write(string("* ") + usernames[i] + string(" ") + buf, uchannels[i]);
}

static int mute(int i, FILE* connstream) {
    cout << _("Got \e[34mmute\e[32m command") << endl;
    string buf;
    int j, k, l;
    char c;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        buf = string("");
        while ((c = fgetc(connstream)) != EOF)
            buf += string(1, c);

        int chindex = chname_to_index(uchannels[i]);
        if (is_in_group(GROUP_MUTELIST, uchannels[i], buf))
            client_write(i, _("The user is already muted on the channel!"));
        else {
            write_chanfile(INCLUDE_MUTED, uchannels[i], buf);
            ftruncate(fileno(connstream), 0);
            client_write(i, string(_("Succesfully muted ")) + buf);
            loadchannels(1);
        }
        return 0;
    }
    return 0;
}

static int unmute(int i, FILE* connstream) {
    cout << _("Got \e[34munmute\e[32m command") << endl;
    string buf;
    int j, k, l;
    char c;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        buf = string("");
        while ((c = fgetc(connstream)) != EOF)
            buf += string(1, c);

        int chindex = chname_to_index(uchannels[i]);
        if (!is_in_group(GROUP_MUTELIST, uchannels[i], buf))
            client_write(i, _("The user is not muted on the channel!"));
        else {
            write_chanfile(EXCLUDE_MUTED, uchannels[i], buf);
            ftruncate(fileno(connstream), 0);
            client_write(i, string(_("Succesfully unmuted ")) + buf);
            loadchannels(1);
        }
        return 0;
    }
}

static int blacklist(int i, FILE* connstream) {
    cout << _("Got \e[34mblacklist\e[32m command") << endl;
    string buf;
    int j, k, l;
    char c;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        buf = string("");
        while ((c = fgetc(connstream)) != EOF)
            buf += string(1, c);

        int chindex = chname_to_index(uchannels[i]);
        if (is_in_group(GROUP_BLACKLIST, uchannels[i], buf))
            client_write(i, _("The user is already blacklisted on the channel!"));
        else {
            write_chanfile(INCLUDE_BLACKLISTED, uchannels[i], buf);
            ftruncate(fileno(connstream), 0);
            client_write(i, string(_("Succesfully blacklisted ")) + buf);
            loadchannels(1);
        }
        return 0;
    }
    return 0;
}

static int deblacklist(int i, FILE* connstream) {
    cout << _("Got \e[34mdeblacklist\e[32m command") << endl;
    string buf;
    int j, k, l;
    char c;
    if (!isop[i])
        client_write(i, _("You are not opped on the channel!"));
    else {
        buf = string("");
        while ((c = fgetc(connstream)) != EOF)
            buf += string(1, c);

        int chindex = chname_to_index(uchannels[i]);
        if (!is_in_group(GROUP_BLACKLIST, uchannels[i], buf))
            client_write(i, _("The user is not blacklisted on the channel!"));
        else {
            write_chanfile(EXCLUDE_BLACKLISTED, uchannels[i], buf);
            ftruncate(fileno(connstream), 0);
            client_write(i, string(_("Succesfully deblacklisted ")) + buf);
            loadchannels(1);
        }
        return 0;
    }
}

static void getblacklist(int i, FILE* connstream) {
    cout << "got \e[34mget blacklist\e[32m command" << endl;
    string out = string("");
    for (int j = 0; j < channels.size(); j++)
        if (!channels[j].chname.compare(uchannels[i]))
            for (int k = 0; k < channels[j].blacklist.size(); k++)
                out += channels[j].blacklist[k] + string(", ");
    out = out.substr(0, out.length() - 2);
    client_write(i, out);
}
static void getwhitelist(int i, FILE* connstream) {
    cout << "got \e[34mget whitelist\e[32m command" << endl;
    string out = string("");
    for (int j = 0; j < channels.size(); j++)
        if (!channels[j].chname.compare(uchannels[i]))
            for (int k = 0; k < channels[j].whitelist.size(); k++)
                if (!channels[j].whitelist[k].compare(usernames[i]))
                    out += string("<b>") + channels[j].whitelist[k] + string("</b>, ");
                else
                    out += channels[j].whitelist[k] + string(", ");
    out = out.substr(0, out.length() - 2);
    client_write(i, out);
}
static void getoplist(int i, FILE* connstream) {
    cout << "got \e[34mget oplist\e[32m command" << endl;
    string out = string("");
    for (int j = 0; j < channels.size(); j++)
        if (!channels[j].chname.compare(uchannels[i]))
            for (int k = 0; k < channels[j].oplist.size(); k++)
                if (!channels[j].oplist[k].compare(usernames[i]))
                    out += string("<b>") + channels[j].oplist[k] + string("</b>, ");
                else
                    out += channels[j].oplist[k] + string(", ");
    out = out.substr(0, out.length() - 2);
    client_write(i, out);
}
static void getmutelist(int i, FILE* connstream) {
    cout << "got \e[34mget mutelist\e[32m command" << endl;
    string out = string("");
    for (int j = 0; j < channels.size(); j++)
        if (!channels[j].chname.compare(uchannels[i]))
            for (int k = 0; k < channels[j].mutelist.size(); k++)
                if (!channels[j].mutelist[k].compare(usernames[i]))
                    out += string("<b>") + channels[j].mutelist[k] + string("</b>, ");
                else
                    out += channels[j].mutelist[k] + string(", ");
    out = out.substr(0, out.length() - 2);
    client_write(i, out);
}
static void getonlist(int i, FILE* connstream) {
    cout << "got \e[34mget onlist\e[32m command" << endl;
    string out = string("");
    for (int j = 0; j < clients.size(); j++)
        if (!uchannels[j].compare(uchannels[i]))
            if (!usernames[j].compare(usernames[i]))
                out += string("<b>") + usernames[j] + string("</b>, ");
            else
                out += usernames[j] + string(", ");
    out = out.substr(0, out.length() - 3);
    client_write(i, out);
}
static void whereami(int i, FILE* connstream) {
    cout << "got \e[34mwhere am i\e[32m command" << endl;
    client_write(i, string("You are chatting on ") + uchannels[i]);
}
#endif