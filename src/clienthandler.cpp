#ifndef _CLIENTHANDLER_CPP
#define _CLIENTHANDLER_CPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <csignal>
#undef max // I wonder where it was defined. It just didn't work without this
#include <vector>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <libintl.h>
#include "verification.cpp"
#include "md5.cpp"
#include "commands.cpp"
#include "metamanager_chan.cpp"

#define _(String) gettext (String)

using namespace std;

const char* motd = _("Welcome to GSKW's RBXChat server, <i>%s</i>!<br />Rules:<ol><li>No links to bit.ly or adf.ly as well as other similar sites.<li>No NSFW content<li>No alternate accounts<li>No spamming<li>If you ever manage to crack the system in anyway, congratulations<li><li>Reports bugs and other issues at <a href='http://github.com/rbxchat'>GitHub</a></ol>");

string help_commands = ".join <channel> - Join a channel. Channel names begin with a #.<br />\
.broadcast <message> - Broadcast a message. You have to be an operator.<br />\
.msg &lt;user&gt; &lt;message&gt; - Send a private message.<br />\
.kick &lt;user&gt; - Kick a user. You have to be an operator.<br />\
.ban &lt;user&gt; &lt;reason&gt; - Ban a user. You have to be an operator.<br />\
.unban &lt;user&gt; - Unban a user. You have to be an operator.<br />\
.op &lt;user&gt; - Make a user an operator on the current channel. You have to be an operator.<br />\
.deop &lt;user&gt; - Remove the a user's operator rights on the current channel. You have to be an operator.<br />\
.whitelist &lt;user&gt; - Make a user whitelisted on the current channel. You have to be an operator.<br />\
.dewhitelist &lt;user&gt; - Remove a user from the channel's whitelist. You have to be an operator.<br />\
.getipaddr &lt;user&gt; - Gets a user's IP address. You have to be an operator.<br />\
.finduser &lt;user&gt; - Find out which channel the user is chatting on.<br />\
.chatas &lt;user&gt; &lt;message&gt; - Makes it look like somebody else chatted the message. You have to be an operator.<br />\
.commandas &lt;user&gt; &lt;command&gt; - Makes it look like somebody else used the command. You have to be an operator.<br />\
.reload - Reloads all the files (channel, ban, authentication). You have to be an operator<br />\
.verify &lt;username&gt; - Gives you a that you need to verify your ROBLOX account.<br />\
.finishverify - Checks for the code in your blurb.<br />\
.robloxacc &lt;user&gt; - Finds a user's ROBLOX account.<br />\
.shutdown &lt;reason&gt; - Shuts down the server. You need to be an operator.<br />\
.createchannel &lt;name&gt; - Creates a channel. The initial operator will be gskw, Temppeliherra and you. You have to be an operator.<br />\
.deletechannel &lt;name&gt; - Deletes a channel. You have to be an operator. <b>Warning: never delete the channel #main!</b><br />\
.listchannels - Shows a list of all channels.<br />\
.me - Use this to tell that you did something. Example: .me is pro -> * <yourusername> is pro<br />\
.mute &lt;name&gt; - Mutes a user on the current channel. You have to be an operator.<br />\
.unmute &lt;name&gt; - Unmutes a user on the current channel. You have to be an operator.<br />\
.blacklist &lt;name&gt; - Blacklists a user on the current channel. You have to be an operator.<br />\
.deblacklist &lt;name&gt; - Deblacklist a user on the current channel. You have to be an operator.<br />\
.getblacklist - Get the current channel's blacklist.<br />\
.getwhitelist - Get the current channel's whitelist.<br />\
.getoplist - Get the current channel's operator list.<br />\
.getmutelist - Get the current channel's mute list.<br />\
.getonlist - Get a list of the current channel's online users.";

void conn_handler(int index) {
    printf(_("Forked thread running for client number %d...\n"), index);

    // Client beginning info handler...
    string buf = string(""), cmpbuf = string(""), vercode = string("");
    int forked, regflag = 0, reject = 1, currarg = 0,i, found = 0, nfound = 0, canverify;
    string arguments[3], currarg2;
    char c;
    const char* clientdata;
    struct authinfo ai;
    recv(clients[index], &c, 1, 0);
    if (c != '\x05') {
    	if (c > '\x05')
    		send(clients[index], "Error: server out of date!", strlen("Error: server out of date!"), 0);
    	else
    		send(clients[index], "Error: client out of date!", strlen("Error: client out of date!"), 0);
    	reject = 2;
    	goto disconnect;
    }
    while (recv(clients[index], &c, 1, 0) > 0) {
        buf += string(1, c);
        if (c == '\x03' || c == '\0') break;
    }
    clientdata = buf.c_str();
    if (clientdata[0] != '\x01' && clientdata[0] != '\x04') {
        cerr << _("Client gave invalid command!") << endl;
        close(clients[index]);
        exit(1);
    }
    else {
        // 1 = login, 4 = register
        for (i = 1; i < strlen(clientdata); i++) {
            if (clientdata[i] == '\x02') {
                arguments[currarg] = currarg2;
                ++currarg;
                currarg2 = string("");
            }
            else if (clientdata[i] == '\x03') {
                arguments[currarg] = currarg2;
                break;
            }
            else
                currarg2 += clientdata[i];
        }
        ai.username = arguments[0];
        ai.password = arguments[1];
        for (i = 0; i < authinfotbl.size(); i++) {
        	struct authinfo currai = authinfotbl[i];
        	if (currai.username == ai.username && currai.password == md5(ai.password)) {
        		if (currai.verified.compare("false"))
        			found = 1;
        		else {
        			found = 1;
        		}
        		ai.vercode = currai.vercode;
        	}
        	else if (currai.username == ai.username)
        		nfound = 1;
        }
        char outfilename[50];
        sprintf(outfilename, "buffers/buffer_out_%d", clients[index]);
        FILE* outstream1;
        outstream1 = fopen(outfilename, "a+");
        FILE* stream;
        FILE* connstream;
        char filename[50];
        sprintf(filename, "buffers/buffer_%d", clients[index]);
        stream = fopen(filename, "a+");
        char connfilename[50];
        sprintf(connfilename, "buffers/connhandler_%d", clients[index]);
        connstream = fopen(connfilename, "a+");
        char charnotallowed[100];
        if (!found) {
        	//cout << _("NOT FOUND") << endl;
        	if (clientdata[0] == '\x04') {
        		if (nfound)
        			goto userex;
        		for (i = 0; i < arguments[0].length(); i++) {
        			if ((int)arguments[0][i] < 0x22 || (int)arguments[0][i] > 0x7E || arguments[0][i] == 0x2D || arguments[0][i] == 0x7C) {
        				cout << _("The character ") << (int)arguments[0][i] << _(" is not allowed.") << endl;
        				sprintf(charnotallowed, _("Your username has a character that is not allowed (0x%X). Allowed characters are in range 0x22 ~ 0x7E. The characters 0x2D and 0x7C are disallowed."), (int)arguments[0][i]);
        				send(clients[index], charnotallowed, strlen(charnotallowed), 0);
        				reject = 1;
        				goto disconnect;
        			}
        		}

        		authfile = fopen(AUTHFILE, "r+");
        		fseek(authfile, 0, SEEK_END);
        		vercode = getactivationcode();
        		fprintf(authfile, "%s|%s|%s|%s|false\n", arguments[0].c_str(), md5(arguments[1]).c_str(), arguments[2].c_str(), vercode.c_str());
        		fflush(authfile);
        		fprintf(connstream, "\x03%s", arguments[0].c_str());
        		fflush(connstream);
        		regflag = 1;
        		
        		goto registered;
        	}
        	cout << _("Invalid login") << endl;
        	send(clients[index], _("Login rejected\n"), strlen(_("Login rejected\n")), 0);
        	reject = 1;
        	goto disconnect;
        }
        else {
	        //cout << _("UN") << arguments[0] << endl << "pw" << arguments[1] << endl;
	        if (clientdata[0] == '\x04') {
	        	userex:
	        	send(clients[index], _("User already exists\n"), strlen(_("User already exists\n")), 0);
	        	reject = 1;
	        	goto disconnect;
	        }

	        registered:
	        char motdbuffer[2000];
	        sprintf(motdbuffer, motd, arguments[0].c_str());
	        send(clients[index], motdbuffer, strlen(motdbuffer), 0);
	        //cout << _("sent motd") << endl;
	        found = -1;
	        for (i = 0; i < baninfotbl.size(); i++) {
	        	struct baninfo currbi = baninfotbl[i];
	        	if (!currbi.username.compare(arguments[0]))
	        		found = i;
	        	//cout << "un = " << currbi.username << " pw = " << currbi.reason << endl;
	        }
	        if (found != -1) {
	        	send(clients[index], (string(_("You are banned from the server. Reason: ")) + baninfotbl[found].reason).c_str(), (string(_("You are banned from the server. Reason: ")) + baninfotbl[found].reason).length(), 0);
				reject = 1;
				goto disconnect;
	        }
	        
	        //string buf3;
	        char* buf2;
	        size_t nbytes;
	        int written;
	        //fprintf(outstream, "&lt;<b>SYSTEM</b>&gt;<i>%s</i> has joined the server!", arguments[0].c_str());
	        if (!regflag)
	        	written = fprintf(connstream, "\x01%s", arguments[0].c_str());
	        else
	        	cout << _("User has registered") << endl;
	        //cout << _("fprint has written to ") << connfilename << endl;
	        //fflush(outstream);
	        fflush(connstream);
	        fseek(outstream1, 0, SEEK_SET);
	        int recvres;
	        //fcntl(clients[index], F_SETFL, O_NONBLOCK);
	        forked = fork();
	        if (!forked) {
	        	cout << _("Inner forked thread running...") << endl;
	        	while(1) {
		            
		            buf2 = (char*) calloc(sizeof(char), 8193);
		            recvres = recv(clients[index], buf2, 8192, 0);
		            cout << "Got: " << buf2 << endl;
		            if (buf2[0] == '\x10') { // ping
		            	send(clients[index], "\x10", 1, MSG_WAITALL); // pong
		            	goto skipwriting;
		            }
		            else
		            	;//cout << _("Got input ") << (int)buf2[0] << endl;
		            //recvres = read(clients[index], buf2, nbytes);
		            if (recvres == 0) {
		            	cout << _("recv error") << endl;
		            	disconnect:
		            	cout << arguments[0] << _(" DISCONNECTED! mode = ") << reject << endl;
		            	if (reject == 1)
		            		fputc('\x04', connstream);
		            	else if (reject == 2)
		            		shutdown(clients[index], 2);
		            	else
		            		fputc('\x02', connstream);
		            	fflush(connstream);
		            	fclose(connstream);
		            	fclose(stream);
		            	fclose(outstream1);
		            	unlink(filename);
		            	unlink(outfilename);
		            	exit(0);
		            }
		            if (!strlen(buf2)) {
		            	free(buf2);
		            	continue;
		            }
		            //cout << _("Recv") << endl;
		            //&buf2 + nbytes = '\0';
		            //buf2[strlen(buf2)] = '\0';
		            //cout << _("Received") << buf2 << "END" << endl;
		            cmpbuf = string(buf2);
		            
		            if (!cmpbuf.compare(".quit")) {
		            	cout << _("quit requested") << endl;
		            	goto disconnect;
		            }
		            if (!cmpbuf.find(".verify ")) {
				        cout << _("Verifying") << endl;
				        string rblxacc = cmpbuf.substr(strlen(".verify "));
				        if (!ai.vercode.length())
				            ai.vercode = vercode;
				        send(clients[index], (string(_("Put the following code to your blurb: ")) + ai.vercode).c_str(), (string(_("Put the following code to your blurb: ")) + ai.vercode).length(), 0);
				        send(clients[index], _("Chat \".finishverify\" when you are done."), strlen(_("Chat \".finishverify\" when you are done.")), 0);
				        ai.rblxun = rblxacc;
				    }

				    else if (!cmpbuf.find(".finishverify")) {
				        cout << _("Finishing verification") << endl;
				        send(clients[index], _("Checking for the code in your blurb..."), strlen(_("Checking for the code in your blurb...")), 0);
				        if (!checkactivationcode(ai.rblxun, ai.vercode))
				            send(clients[index], _("The code was not found in your blurb!"), strlen(_("The code was not found in your blurb!")), 0);
				        else {
				            send(clients[index], _("The code was found, your account is now verified!"), strlen(_("The code was found, your accoutn is now verified!")), 0);
				            FILE* fp = fopen(AUTHFILE, "a+");
				            ftruncate(fileno(fp), 0);
				            for (i = 0; i < authinfotbl.size(); i++) {
				                if (!authinfotbl[i].username.compare(ai.username))
				                    fprintf(fp, "%s|%s|%s||true\n", ai.username.c_str(), ai.password.c_str(), ai.rblxun.c_str());
				                else
				                    fprintf(fp, "%s|%s|%s|%s|%s\n", authinfotbl[i].username.c_str(), authinfotbl[i].password.c_str(), authinfotbl[i].rblxun.c_str(), authinfotbl[i].vercode.c_str(), authinfotbl[i].verified.c_str());
				            }
				            fclose(fp);
				        }
				        cout << _("Done") << endl;
				    }
				        else if (!cmpbuf.find(".help")) {
				        cout << _("Getting help") << endl;
				        send(clients[index], help_commands.c_str(), help_commands.length(), 0);
				    }
		            else if (!cmpbuf.find("."))
		            	evaluate_command(connstream, cmpbuf);
		            else {
		            	written = fprintf(outstream1, "&lt;%s&gt; %s", arguments[0].c_str(), buf2);
		            	fflush(outstream1);
		            }


		            if (written < 0)
		                cerr << ferror(outstream1) << endl;
		            else if (written != 0) {
			            skipwriting:
			            free(buf2);
			            fseek(outstream1, 0, SEEK_SET);
			        }

		            //buf3 = string("");
		            /*while (recv(clients[index], &buf2, 1, 0) > 0) {
		                //cout << _("Receiving") << buf2 << endl;
		                if (buf2 == 0x0A || buf2 == 0x00) break;
		                buf3 += buf2;
		            }
		            if (buf3.length()) {
		                written = fprintf(outstream, buf3.c_str());
		                //cout << _("Writing ") << buf3.c_str() << endl;
		                fflush(outstream);
		                if (written < 0)
		                    cerr << ferror(outstream) << endl;
		            }*/
		            //cout << _("LEN ") << buf3.length() << endl;
		        }
	        }
	        while (1) {
	            //cout << _("LOOPED") << endl;
	            //usleep(100);
	            // Open file
	            //cout << filename << endl;
	            fseek(stream, 0, SEEK_SET);
	            buf = string("");

	            // Get pending stuff
	            while ((c = fgetc(stream)) != EOF)
	                buf += string(1, c);
	            if (buf.length()) {
	                //cout << _("hi") << endl;
	                send(clients[index], buf.c_str(), buf.length(), 0);
	                //cout << _("error ") << numclients << endl;
	                //cout << _("sent ") << buf.c_str() << endl;
	                ftruncate(fileno(stream), 0);
	            }
	            //cout << _("asdloop");

	            //cout << _("Might recv") << endl;

	            // Receiving
	        }
	    }
	}
}
#endif