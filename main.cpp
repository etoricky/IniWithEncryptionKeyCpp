#include "Config.h"

void main(int argc, char *argv[]) {
	Config::inst().setEncryptKeyToPattern("dr56tfgy7", "Password"); // fixed key
	Config::inst().parseFile(argv[1]);
}