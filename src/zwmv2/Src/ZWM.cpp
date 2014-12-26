#include "SearchEngine.h"

using namespace std;

int main(int argc, char **argv)
{
	CSearchEngine schEgn;

	schEgn.RunEngine(argc > 1 ? argv[1] : "");

	return 0;
}