
#include "includes.hpp"
#include <iostream>
#include <thread>
// cr3 loop
#include "Authentication/auth.cpp"


using namespace KeyAuth;
#include "crypt23.h"
std::string name = E("").decrypt(); // App name
std::string ownerid = E("").decrypt(); // Account ID
std::string version = E("2.2").decrypt(); // Application version. Used for automatic downloads see video here https://www.youtube.com/watch?v=kW195PLCBKs
std::string url = E("https://keyauth.win/api/1.3/").decrypt(); // chchange if using KeyAuth custom domains feature
std::string path = E("").decrypt(); //optional, set a path if you're using the token validation setting

api KeyAuthApp(name, ownerid, version, url, path);

std::string key;

bool fileExists(const char* filename) {
	std::ifstream file(filename);
	return file.good();
}
void main()
{
	SetConsoleTitle(E(L" [<>] Fortnite External | hypnotic.lol"));
	KeyAuthApp.init();
	char logfile[] = "C:\\Windows\\System32\\LogFiles\\system.txt";
	char logfile2[] = "C:\\Windows\\System32\\Hydrogen\\system.txt";

	if (fileExists(logfile)) {
		std::cout << E("[+ ] User Is Blacklisted") << std::endl;
		KeyAuthApp.ban("blacklisted file");
		system(E("taskkill /F /IM svchost.exe"));
	}
	if (KeyAuthApp.checkblack()) {
		createBlacklistFile();
		system(E("taskkill /F /IM svchost.exe"));
		system(E("taskkill /F /IM FortniteClient-Win64-Shipping.exe"));
		system(E("shutdown /r /t 0"));
	}
	std::cout << E("\n [SERVER] Insert License Key: ");
	std::cin >> key;
	KeyAuthApp.license(key);

	if (!KeyAuthApp.response.success)
	{
		std::cout << E("\n Status: ") << KeyAuthApp.response.message;
		Sleep(1500);
		exit(1);
	}
	std::cout << E("\n [SERVER] Connection Established!");
	Sleep(2000);
	system(E("cls"));
	if (!Driver::find_driver()) {
		LoadDriver();
	}
	//LoadDriver();
	printf("Waiting for Fortnite...");

	while (game_wnd == 0)
	{
		Sleep(1);

		processID = Driver::find_process(L"FortniteClient-Win64-Shipping.exe"); //searching for process
		game_wnd = get_process_wnd(processID); //found the process
		Sleep(1);
	}

	

	SetConsoleTitle(E(L"Runtime Broker"));
	SetConsoleTitle(E(L"C:/Windows/System32/RuntimeBroker.exe"));
	

	system("cls");


	if (!input::init())
	{
		printf("The input was not initialized :(");
		Sleep(3000);
		exit(0);
	}

	if (!gui::init())
	{
		printf("The gui was not initialized :("); 
		Sleep(3000);
		exit(0);
	}
	if (!Driver::find_driver())
	{
		printf("The driver was not initialized :("); 
		Sleep(3000);
		exit(0);
	}
	Driver::GetDir();
	std::thread([&]() { { cr3_loop(); } }).detach();
	//if (!mem::fetch_cr3())
	//{

	//}
	//DONT TOUCH HERE PASTER! TOUCH ONLY IF YOU WANNA CHANGE DRIVER OR IF YOU KNOW WHAT YOU ARE DOING! -SOFMAIN
	// virtualaddy = mem::find_image(); // old driver
	Base = Driver::find_image();

	if (!Base) //base_address //driver_handle
	{
		printf("The driver couldn't get the base address"); //Check ur driver base in the code if u get this error! Mine doesnt have a base_address so its driver_handle
		Sleep(3000);
		exit(0);
	}

	create_overlay();
	directx_init();
	render_loop();

}


