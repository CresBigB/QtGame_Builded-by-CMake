#include "GameEngin.h"
#include "GameStartWindow.h"
#include "Headers.h"

#pragma comment(linker,"/subSystem:console /entry:mainCRTStartup")

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	GameStartWindow window;
	window.show();

	return QApplication::exec();
}

//#include "main.moc"