#include "global.h"
#include "MeshLoader.h"

// The MAIN function, from here we start the application and run the game loop
int main()
{
	MeshLoader* meshLoader = new MeshLoader();
	meshLoader->LoadObj("c://test.obj");
	int i;
	cin >> i;
	return 0;
}

