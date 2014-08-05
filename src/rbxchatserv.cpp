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
#include <sstream>
#include <algorithm>
#include <libintl.h>
#include "clienthandler.cpp"
#include "verification.cpp"
#include "commands.cpp"

using namespace std;

#if DEBUG
    #if !defined(PORT)
        #define PORT "998"
   #endif
#else
    #if !defined(PORT)
        #define PORT "556"
   #endif
#endif
#if !defined(BACKLOG)
    #define BACKLOG 50
#endif
#if !defined(_)
    #define _(String) gettext (String)
#endif





void sigchld_handler(int s) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
}



void* get_in_addr(struct sockaddr* sa) {
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int array_push(vector<int> arr, int elem) {
	arr.push_back(elem);
    return numclients - 1;
}

void strarray_push(vector<string> arr, string elem) {
	arr.push_back(elem);
}




int main(int argc, char* argv[]) {
    cout << "Trying to start server..." << endl;
    const char* port = argv[1];
    long portlong = 0L;
    if ((portlong = atol(port)) == 0L) {
        cout << "\e[31mError: invalid port!\e[0m" << endl;
        exit(1);
    }
	printf(_("\e[1;32mStarting server at \e[33mlocalhost:%s\e[32m\n"), port);
	signal(SIGINT, cleanup);
	clients.reserve(1000);
	usernames.reserve(1000);
	isop.reserve(1000);
	struct sockaddr_in ip4addr;
	int sockfd, sin_size, new_fd, fdmax;
	struct sockaddr_storage their_addr;
	char s[INET6_ADDRSTRLEN];
    fd_set master, read_fds;
    if (argc != 2) {
        cout << "\e[31mError: invalid number of arguments.\e[0m";
        exit(1);
    }

	ip4addr.sin_family = AF_INET;
	ip4addr.sin_port = htons(portlong);
	ip4addr.sin_addr.s_addr = INADDR_ANY;
	inet_pton(AF_INET, "0.0.0.0", &ip4addr.sin_addr); // Listen on current device

	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if ((bind(sockfd, (struct sockaddr*)&ip4addr, sizeof ip4addr)) < 0) {
		perror("\e[31mbind");
        cout << "\e[0m";
		exit(1);
	}
	if (listen(sockfd, BACKLOG) == -1) {
		perror("\e[31mlisten");
        cout << "\e[0m";
		exit(1);
	}

    FD_SET(sockfd, &master);
    fdmax = sockfd;

    char filename[50];
    char connfilename[50];
    //sprintf(filename, "buffers/buffer_out_%d", numclients - 1);
    string buf, msgbuf;
    FILE* stream;
    FILE* connstream;
    char c, cmd;
    int i, err, j, k, chfound, l, istmuted, mode;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10;
    loadchannels();
    loadauth();
    loadbans();
    write_chanfile();
    cout << _("Waiting for connections...") << endl;

    while(1) {
        for (i = 0; i < numclients; i++) {
            //cout << "LOOP DA WOOP" << endl;
            sprintf(connfilename, "buffers/connhandler_%d", clients[i]);
            connstream = fopen(connfilename, "a+");
            if (connstream == NULL) {
            	//cerr << strerror(errno) << endl;
            	//fclose(connstream);
		        //perror("\e[31mfopen connstream");
                //cout << "\e[0m";
            	continue;
            }
            //cout << "past" << endl;
            fseek(connstream, 0, SEEK_SET); // DO NOT TOUCH!
            c = fgetc(connstream);
            switch (c) {
                case EOF:
                	err = ferror(connstream);
                	if (err) {
                		cerr << strerror(err) << endl;
                		exit(1);
                	}
                	fclose(connstream);
                	goto eof;
                case CMD_LEFT:
                case CMD_REJECTED:
                	disconnect(i, c, connstream, connfilename);
                    continue;
                case CMD_JOINED:
                case CMD_REGISTERED:
            	   join(i, c, connstream);
                   break;
                case CMD_CHANGE_CHANNEL:
                    if (change_channel(i, connstream))
                        goto eof;
                    break;
                case CMD_BROADCAST:
                    broadcast(i, connstream);
                    break;
                case CMD_PRIVATE_MESSAGE:
                    pm(i, connstream);
                    break;
                case CMD_KICK:
                    kick(i, connstream);
                    break;
                case CMD_BAN:
                    ban(i, connstream);
                    break;
                case CMD_UNBAN:
                    unban(i, connstream);
                    break;
                case CMD_OP:
                    if(op(i, connstream)) // TODO: Possibly remove this?
                        goto eof;
                    break;
                case CMD_DEOP:
                    if (deop(i, connstream)) // TODO: Possibly remove this?
                        goto eof;
                    break;
                case CMD_WHITELIST:
                    if (whitelist(i, connstream)) // TODO: Possibly remove this?
                        goto eof;
                    break;
                case CMD_DEWHITELIST:
                    if (dewhitelist(i, connstream)) // TODO: Possibly remove this?
                        goto eof;
                    break;
                case CMD_GET_IP_ADDRESS:
                    getipaddr(i, connstream);
                    break;
                case CMD_FIND_USER:
                    finduser(i, connstream);
                    break;
                case CMD_CHAT_AS:
                    chatas(i, connstream);
                    break;
                case CMD_COMMAND_AS:
                    commandas(i, connstream);
                    break;
                case CMD_RELOAD:
                    reload(i, connstream);
                    break;
                case CMD_FIND_ROBLOX_ACCOUNT:
                    findrobloxaccount(i, connstream);
                    break;
                case CMD_CREATE_CHANNEL:
                    createchannel(i, connstream);
                    break;
                case CMD_DELETE_CHANNEL:
                    deletechannel(i, connstream);
                    break;
                case CMD_LIST_CHANNELS:
                    listchannels(i, connstream);
                    break;
                case CMD_ME:
                    me(i, connstream);
                    break;
                case CMD_MUTE:
                    if (mute(i, connstream))
                        goto eof;
                    break;
                case CMD_UNMUTE:
                    if (unmute(i, connstream))
                        goto eof;
                    break;
                case CMD_BLACKLIST:
                    if (blacklist(i, connstream))
                        goto eof;
                    break;
                case CMD_DEBLACKLIST:
                    if (deblacklist(i, connstream))
                        goto eof;
                    break;
                case CMD_GET_BLACKLIST:
                    getblacklist(i, connstream);
                    break;
                case CMD_GET_WHITELIST:
                    getwhitelist(i, connstream);
                    break;
                case CMD_GET_OPLIST:
                    getoplist(i, connstream);
                    break;
                case CMD_GET_MUTELIST:
                    getmutelist(i, connstream);
                    break;
                case CMD_GET_ONLINE_LIST:
                    getonlist(i, connstream);
                    break;
                case CMD_WHERE_AM_I:
                    whereami(i, connstream);
                    break;
                default:
                	cout << _("Got other command ") << (int)c << endl;
                    break;
            }
            ftruncate(fileno(connstream), 0);
            eof:
    	    if (!ferror(connstream))
    	        fclose(connstream);

            sprintf(filename, "buffers/buffer_out_%d", clients[i]);
            stream = fopen(filename, "a+");
            if (stream == NULL) 
                cerr << strerror(errno) << endl;
            buf = string("");
            fseek(stream, 0, SEEK_SET);
            while ((c = fgetc(stream)) != EOF)
                buf += string(1, c);
            if (buf.length()) {
                //cout << buf.length() << buf.c_str();
                //cout << buf << endl;
                if (ismuted[i])
                    client_write(i, "You are muted on the channel!");
                else
                    channel_write(buf, uchannels[i]);
                ftruncate(fileno(stream), 0);
                fseek(stream, 0, SEEK_SET);
            }
            fclose(stream);
            
        }
		sin_size = sizeof their_addr;
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, &timeout) == -1) {
            perror("select");
            exit(4);
        }
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == sockfd) {
                    // handle new connections
                    sin_size = sizeof their_addr;
                    new_fd = accept(sockfd,
                        (struct sockaddr *)&their_addr,
                        (socklen_t*)&sin_size);

                    if (new_fd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(new_fd, &master); // add to master set
                        if (new_fd > fdmax) {    // keep track of the max
                            fdmax = new_fd;
                        }
                        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr), s, sizeof s);
                        printf(_("Connected: %s\n"), s);
                        ++numclients;
                        //pipe(pipes[numclients]);

                        //FILE* out;
                        //out = fopen(filename, "w");
                        //cout << "clients " << clients[numclients];
                        clients.push_back(new_fd);
                        ipaddrs.push_back(string(s));

                        int forked = fork();
                        if (!forked)
                            conn_handler(numclients - 1);
                        else if (forked == -1)
                            perror(_("FORK FAILURE!"));
                        else
                            //global_write("Somebody has joined!\n");
                            ;
                    }
                }
            } // END got new incoming connection
        } // END looping through file descriptors

        //new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size); Not using accept() as it waits for the client to connect.
        //cout << "accepted" << endl;
	}
}
