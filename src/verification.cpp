#ifndef _VERIFICATION_CPP
#define _VERIFICATION_CPP
#include <iostream>
#include <string>

using namespace std;

static string getactivationcode() {
	FILE* fp;
	char c;
	//cout << "getactivationcode" << endl;
	fp = popen("/cygdrive/c/xampp/php/php -f getactivationcode.php", "r");
	if (fp == NULL) {
		perror("popen");
		return string("ERROR");
	}
	else {
		string buf;
		while((c = fgetc(fp)) != EOF)
			buf += string(1, c);
		pclose(fp);
		cout << "The activation code is " << buf << endl;
		return buf;
	}
}

static int checkactivationcode(string username, string authcode) {
	FILE* fp;
	char c;
	fp = popen((string("/cygdrive/c/xampp/php/php -f checkverificationcode.php \"") + username + string("\" \"") + authcode + string("\"")).c_str(), "r");
	if (fp == NULL) {
		perror("popen");
		return 0;
	}
	else {
		string buf;
		while ((c = fgetc(fp)) != EOF)
			buf += string(1, c);
		pclose(fp);
		cout << "It outputs " << buf << endl;
		if (!buf.compare("true"))
			return 1;
		return 0;
	}
}
#endif