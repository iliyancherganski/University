#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <conio.h> 

using namespace std;

const int NAME_LENGTH = 20;
const int SERVICE_TYPE_NAME = 50;
const int ORDER_COUNT = 100;
const int SERVICE_COUNT = 100;
const int PROBLEM_OF_CLIENT_LENGTH = 50;
const int SERIAL_NUMBER_LENGTH = 10;
const char ORDER_FILE_NAME[] = "orders.bin";
const char SERVICE_FILE_NAME[] = "services.bin";

const int COLUMN_1 = 3; // order number
const int COLUMN_2 = 3; // day of the month
const int COLUMN_3 = NAME_LENGTH; // client name
const int COLUMN_4 = 11; // device type
const int COLUMN_5 = SERIAL_NUMBER_LENGTH + 1; // serial number
const int COLUMN_6 = PROBLEM_OF_CLIENT_LENGTH - 20; // problem of client
const int COLUMN_7 = NAME_LENGTH; // technician name
const int COLUMN_8 = SERVICE_TYPE_NAME - 30; // service type
const int COLUMN_9 = 10; // order type
const int COLUMN_10 = 15; // normal price for service
const int COLUMN_11 = 11; // status
const int COLUMN_12 = 15; // total price
const int COLUMN_TOTAL1 = COLUMN_1 + COLUMN_2 + COLUMN_3 + COLUMN_4 + COLUMN_5 + COLUMN_6 + COLUMN_7 + COLUMN_8 + COLUMN_9 + COLUMN_10 + COLUMN_11 + COLUMN_12 + 11 * 3; // total price

//		  COLUMN_1  => service number
//		  COLUMN_8	=> service type
//		  COLUMN_10 => service price
const int COLUMN_TOTAL2 = COLUMN_1 + SERVICE_TYPE_NAME + COLUMN_10 + 2 * 3;

struct Service
{
	int serviceNumber = -1;
	char serviceTypeName[SERVICE_TYPE_NAME] = " ";
	double priceIncrease = 1;
	double priceForService = 0;
	int daysOfService = 0;
};
enum OrderType
{
	EXPRESS = 0,
	FAST = 1,
	STANDARD = 2,
};
enum DeviceType
{
	LAPTOP = 0,
	PC = 1,
	SMARTPHONE = 2,
	TABLET = 3,
	PRINTER = 4,
};
enum OrderStatus
{
	ACCEPTED = 0,
	REJECTED = 1,
	COMPLETED = 2,
};
struct Order
{
	int orderNumber = -1;
	int dayOfMonth = 0;
	char clientName[NAME_LENGTH] = " ";
	char serialNumber[SERIAL_NUMBER_LENGTH] = " ";
	DeviceType deviceType = LAPTOP;
	char problemOfClient[PROBLEM_OF_CLIENT_LENGTH] = " ";
	OrderStatus status = ACCEPTED;

	char technicianName[NAME_LENGTH] = " ";

	Service service;
	OrderType orderType = STANDARD;
	double totalPrice = 0;
};

// Visual
void printMenu();
void printExitScreen();
void printOrderTableHeader();
void printOrderRow(Order order);
void clearConsole();

// Lists
void printAllOrders(Order orders[]);
void printAllDevicesByType(Order orders[]);
void printAllDevicesByStatus(Order orders[]);
void printAllOrdersWithProblem(Order orders[]);
void sortOrdersByDate(Order orders[], bool haveTechnicianFilter);
void printAllServices();

// Adding/updating
void addOrder(Order orders[]);
void updateOrder(Order orders[]);
void addService(); // to-do

// Specialised inputs
void inputOrderType(Order& order);
void inputOrderStatus(Order& order);

// Additional
int getOrderCount(Order orders[]);
int getServiceCount(Service services[]);
Service getServiceByServiceNumber(int serviceNumber);
void clearBuffer();

// File mamagement
void createFileIfNotExist();
void readOrdersFromFile(Order orders[]);
void rewriteOrdersInFile(Order orders[]);
void readServicesFromFile(Service services[]);
void rewriteServicesInFile(Service services[]);

// Enum converters and string manipulations
string truncate(char* str, int width);
char* deviceTypeToString(DeviceType deviceType);
char* orderTypeToString(OrderType orderType);
char* orderStatusToString(OrderStatus orderStatus);

int main()
{
	Order orders[ORDER_COUNT];
	readOrdersFromFile(orders);

	createFileIfNotExist();

	int command1 = 0;
	bool exit = false;
	while (!exit)
	{
		printMenu();
		cout << "Input command: ";
		cin >> command1;
		if (cin.fail())
		{
			command1 = -1;
			clearBuffer();
		}
		switch (command1)
		{
		case 0:
			clearConsole();
			break;
		case 1:
			clearConsole();
			addOrder(orders);
			clearConsole();
			break;
		case 2:
			clearConsole();
			printAllOrders(orders);
			break;
		case 3:
			clearConsole();
			printAllDevicesByType(orders);
			break;
		case 4:
			clearConsole();
			printAllDevicesByStatus(orders);
			break;
		case 5:
			clearConsole();
			sortOrdersByDate(orders, false);
			break;
		case 6:
			clearConsole();
			sortOrdersByDate(orders, true);
			break;
		case 7:
			clearConsole();
			printAllOrdersWithProblem(orders);
			break;
		case 8:
			clearConsole();
			updateOrder(orders);
			break;
		case 9:
			clearConsole();
			addService();
			clearConsole();
			break;
		case 10:
			clearConsole();
			printAllServices();
			break;
		case 11:
			exit = true;
			printExitScreen();
			break;
		default:
			clearConsole();
			cout << "Invalid command! Please try again!" << endl;
			break;
		}
	}
}

// Visual
void printMenu()
{
	cout << " ===== MENU ===== " << endl;
	cout << "0.  Clear console" << endl << endl;

	cout << "1.  Add orders" << endl; // B.a
	cout << "2.  Print all orders" << endl; // C.a
	cout << "3.  Print all devices of type" << endl;// D.a
	cout << "4.  Print all devices by status" << endl; // D.b
	cout << "5.  Print sorted orders by date" << endl; // E.a
	cout << "6.  Print all completed orders of a technician (and order them by date of accepting)" << endl; // G.a
	cout << "7.  Print all orders with a specific problem" << endl; // G.b
	cout << "8.  Update order type of a specific order (by order number)" << endl << endl; // H.a, H.b, I

	cout << "9.  Add service" << endl;
	cout << "10. Print all services" << endl;
	cout << "11. Exit" << endl;
}
void printExitScreen()
{
	clearConsole();
	cout << endl << " ==================================== " << endl;
	cout << endl << " === YOU HAVE EXITED THE PROGRAM. === " << endl;
	cout << endl << " ==================================== " << endl;
}
void printOrderTableHeader()
{
	cout << setw(COLUMN_1) << "No" << " | "
		<< setw(COLUMN_2) << "Day" << " | "
		<< setw(COLUMN_3) << "Client name" << " | "
		<< setw(COLUMN_4) << "Device type" << " | "
		<< setw(COLUMN_5) << "Serial No" << " | "
		<< setw(COLUMN_6) << "Problem of client" << " | "
		<< setw(COLUMN_7) << "Technician name" << " | "
		<< setw(COLUMN_8) << "Service type" << " | "
		<< setw(COLUMN_9) << "Order type" << " | "
		<< setw(COLUMN_10) << "Standard price" << " | "
		<< setw(COLUMN_11) << "Status" << " | "
		<< setw(COLUMN_12) << "Total price" << endl;
	for (int i = 0; i <= COLUMN_TOTAL1; i++)
	{
		cout << "=";
	}
	cout << endl;
}
void printOrderRow(Order order)
{
	cout << setw(COLUMN_1) << order.orderNumber << " | ";
	/*if (order.status == COMPLETED)
	{
		cout << setw(COLUMN_1) << order.orderNumber << " | ";
	}
	else
	{
		cout << setw(COLUMN_1) << " - " << " | ";
	}*/
	cout << setw(COLUMN_2) << order.dayOfMonth << " | "
		<< setw(COLUMN_3) << truncate(order.clientName, COLUMN_3) << " | "
		<< setw(COLUMN_4) << truncate(deviceTypeToString(order.deviceType), COLUMN_4) << " | "
		<< setw(COLUMN_5) << truncate(order.serialNumber, COLUMN_5) << " | "
		<< setw(COLUMN_6) << truncate(order.problemOfClient, COLUMN_6) << " | ";

	if (order.status == COMPLETED)
	{
		cout.setf(ios::fixed);
		cout.setf(ios::showpoint);
		cout.precision(2);

		cout << setw(COLUMN_7) << truncate(order.technicianName, COLUMN_7) << " | "
			<< setw(COLUMN_8) << truncate(order.service.serviceTypeName, COLUMN_8) << " | "
			<< setw(COLUMN_9) << truncate(orderTypeToString(order.orderType), COLUMN_9) << " | "
			<< setw(COLUMN_10 - 4) << order.service.priceForService << " lv." << " | "
			<< setw(COLUMN_11) << truncate(orderStatusToString(order.status), COLUMN_11) << " | "
			<< setw(COLUMN_12 - 4) << order.totalPrice << " lv." << endl;
	}
	else
	{
		cout << setw(COLUMN_7) << " - " << " | "
			<< setw(COLUMN_8) << " - " << " | "
			<< setw(COLUMN_9) << truncate(orderTypeToString(order.orderType), COLUMN_9) << " | "
			<< setw(COLUMN_10) << " - " << " | "
			<< setw(COLUMN_11) << truncate(orderStatusToString(order.status), COLUMN_11) << " | "
			<< setw(COLUMN_12) << " - " << endl;
	}
}
void clearConsole()
{
	system("cls");
}

// Lists
void printAllOrders(Order orders[])
{
	int orderCount = getOrderCount(orders);

	cout << " === ALL ORDERS === \n" << endl;

	printOrderTableHeader();

	for (int i = 0; i < orderCount; i++)
	{
		printOrderRow(orders[i]);
	}
	cout << endl << endl;
}
void printAllDevicesByType(Order orders[])
{
	DeviceType type = LAPTOP;
	int command = 0;
	bool exit = false;
	while (!exit)
	{
		cout << "= Types of devices =\n";
		cout << "	0.LAPTOP\n";
		cout << "	1.PC \n";
		cout << "	2.SMARTPHONE \n";
		cout << "	3.TABLET \n";
		cout << "	4.PRINTER \n";
		cout << "Input the type of device: ";
		cin >> command;
		if (cin.fail())
		{
			cout << "Invalid input! Please try again!" << endl;
			clearBuffer();
			continue;
		}
		if (command >= 0 && command <= 4)
		{
			type = (DeviceType)command;
			exit = true;
		}
		else
		{
			cout << "Invalid device type! Please try again!" << endl;
		}

	}

	int orderCount = getOrderCount(orders);

	cout << " === DEVICES OF TYPE: " << deviceTypeToString(type) << " === \n" << endl;

	printOrderTableHeader();

	for (int i = 0; i < orderCount; i++)
	{
		if (orders[i].deviceType == type)
		{
			printOrderRow(orders[i]);
		}
	}
	cout << endl << endl;
}
void printAllDevicesByStatus(Order orders[])
{
	OrderStatus status = ACCEPTED;
	int command = 0;
	bool exit = false;
	while (!exit)
	{
		cout << "= STATUS =" << endl;
		cout << "	0. ACCEPTED" << endl;
		cout << "	1. REJECTED" << endl;
		cout << "	2. COMPLETED" << endl;
		cout << "Input status: ";
		cin >> command;
		if (cin.fail())
		{
			cout << "Invalid input! Please try again!" << endl;
			clearBuffer();
			continue;
		}
		if (command >= 0 && command <= 2)
		{
			status = (OrderStatus)command;
			exit = true;
		}
		else
		{
			cout << "Invalid device type! Please try again!" << endl;
		}

	}

	int orderCount = getOrderCount(orders);

	cout << " === DEVICES OF STATUS: " << orderStatusToString(status) << " === \n" << endl;

	printOrderTableHeader();

	for (int i = 0; i < orderCount; i++)
	{
		if (orders[i].status == status)
		{
			printOrderRow(orders[i]);
		}
	}
	cout << endl << endl;
}
void printAllOrdersWithProblem(Order orders[])
{
	clearBuffer();
	string problem;
	cout << "Input the problem of the client: ";
	getline(cin, problem);
	if (problem.length() > PROBLEM_OF_CLIENT_LENGTH)
	{
		problem = problem.substr(0, PROBLEM_OF_CLIENT_LENGTH);
	}


	int orderCount = getOrderCount(orders);

	cout << " === ORDERS WITH PROBLEM: " << problem << " === \n" << endl;

	printOrderTableHeader();

	string temp;
	for (int i = 0; i < orderCount; i++)
	{
		temp = orders[i].problemOfClient;
		temp = temp.substr(0, problem.length());
		if (temp.compare(problem) == 0)
		{
			printOrderRow(orders[i]);
		}
	}
	cout << endl << endl;
}
void sortOrdersByDate(Order orders[], bool haveTechnicianFilter)
{
	readOrdersFromFile(orders);

	int orderCount = getOrderCount(orders);
	int flag = false;
	Order temp;
	for (int i = 0; i < orderCount; i++)
	{
		flag = false;
		for (int j = i + 1; j < orderCount; j++)
		{
			if (orders[i].dayOfMonth > orders[j].dayOfMonth)
			{
				flag = true;
				temp = orders[i];
				orders[i] = orders[j];
				orders[j] = temp;
			}
		}
		if (!flag)
		{
			break;
		}
	}

	if (haveTechnicianFilter)
	{
		cout << "Input the name of the technician: ";
		char technicianName[NAME_LENGTH];
		cin.getline(technicianName, NAME_LENGTH);
		clearBuffer();

		cout << " === SORTED ORDERS BY DATE FOR TECHNICIAN: " << technicianName << " === \n" << endl;
		printOrderTableHeader();

		int count = 0;

		for (int i = 0; i < orderCount; i++)
		{
			if (orders[i].status == COMPLETED &&
				strcmp(orders[i].technicianName, technicianName) == 0)
			{
				printOrderRow(orders[count++]);
			}
		}
	}
	else
	{
		cout << " === SORTED ORDERS BY DATE === \n" << endl;
		printOrderTableHeader();

		for (int i = 0; i < orderCount; i++)
		{
			printOrderRow(orders[i]);
		}
	}
	cout << endl << endl;

	readOrdersFromFile(orders);
}
void printAllServices()
{
	Service services[SERVICE_COUNT];
	cout << " === ALL SERVICES === " << endl;
	readServicesFromFile(services);
	cout << setw(COLUMN_1) << "No" << " | "
		<< setw(SERVICE_TYPE_NAME) << "Type" << " | "
		<< setw(COLUMN_10) << "Standard price" << endl;
	for (int i = 0; i <= COLUMN_TOTAL2; i++)
	{
		cout << "=";
	}
	cout << endl;
	int count = getServiceCount(services);
	cout.setf(ios::fixed);
	cout.setf(ios::showpoint);
	cout.precision(2);
	for (int i = 0; i < count; i++)
	{
		cout << setw(COLUMN_1) << services[i].serviceNumber << " | "
			<< setw(SERVICE_TYPE_NAME) << truncate(services[i].serviceTypeName, SERVICE_TYPE_NAME) << " | "
			<< setw(COLUMN_10 - 4) << services[i].priceForService << " lv." << endl;
	}
	cout << endl << endl;
}

// Adding/updating
void addOrder(Order orders[])
{
	readOrdersFromFile(orders);
	cout << " === ADD ORDER ===\n\n";
	int ordersCount = getOrderCount(orders);
	int count = 0;
	bool exit = false;
	while (!exit)
	{
		cout << "There is space for " << ORDER_COUNT - ordersCount << " more orders. \n";
		cout << "Input the number of orders that you want to add: ";
		cin >> count;
		if (cin.fail())
		{
			cout << "Invalid input! Please try again!" << endl;
			clearBuffer();
			continue;
		}
		clearBuffer();

		if (ORDER_COUNT - ordersCount < count)
		{
			cout << "Too many orders to add! Please write a lesser number!" << endl;
			continue;
		}
		exit = true;
	}

	Order order;
	int command = 0;
	int curr = 1;
	for (int i = ordersCount; i < ordersCount + count; i++)
	{
		cout << "\n== Order " << curr++ << " ==\n";

		order.orderNumber = i + 1;

		exit = false;
		while (!exit)
		{
			cout << "Input the day of the month: ";
			cin >> order.dayOfMonth;
			clearBuffer();
			if (order.dayOfMonth >= 1 && order.dayOfMonth <= 31)
			{
				exit = true;
			}
			else
			{
				cout << "Invalid day of the month! Please try again!" << endl;
			}
		}

		cout << "Input name of the client: ";
		cin.getline(order.clientName, NAME_LENGTH);

		exit = false;
		while (!exit)
		{
			cout << "= Types of devices =\n";
			cout << "	0.LAPTOP\n";
			cout << "	1.PC \n";
			cout << "	2.SMARTPHONE \n";
			cout << "	3.TABLET \n";
			cout << "	4.PRINTER \n";
			cout << "Input the type of device: ";
			cin >> command;
			if (cin.fail())
			{
				cout << "Invalid input! Please try again!" << endl;
				clearBuffer();
				continue;
			}

			clearBuffer();
			if (command >= 0 && command <= 4)
			{
				order.deviceType = (DeviceType)command;
				exit = true;
			}
			else
			{
				cout << "Invalid device type! Please try again!" << endl;
			}
		}

		while (true)
		{
			cout << "Input the serial number of the device: ";
			cin >> order.serialNumber;
			if (cin.fail())
			{
				cout << "Invalid input! Please try again!" << endl;
				clearBuffer();
				continue;
			}
			break;
		}

		clearBuffer();

		cout << "What is the problem with the device? - ";
		cin.getline(order.problemOfClient, PROBLEM_OF_CLIENT_LENGTH);

		inputOrderType(order);

		order.status = ACCEPTED;

		orders[i] = order;
	}
	rewriteOrdersInFile(orders);
}
void updateOrder(Order orders[])
{
	readOrdersFromFile(orders);
	printAllOrders(orders);

	int orderCount = getOrderCount(orders);

	int orderNumber = 0;
	int index = 0;
	while (true)
	{
		cout << "Input the order number: ";
		cin >> orderNumber;
		if (cin.fail())
		{
			cout << "Invalid input! Please try again!" << endl;
			clearBuffer();
			continue;
		}
		break;
	}
	clearBuffer();

	bool flag = false;
	Order order;
	for (int i = 0; i < orderCount; i++)
	{
		if (orders[i].orderNumber == orderNumber)
		{
			flag = true;
			index = i;
			order = orders[index];
			break;
		}
	}
	if (!flag)
	{
		cout << "Order with number " << orderNumber << " does not exist!" << endl;
		return;
	}
	clearConsole();

	printOrderTableHeader();
	printOrderRow(order);
	if (order.status == COMPLETED || order.status == REJECTED)
	{
		cout << endl << "Order number " << order.orderNumber << " cannot be modified." << endl << endl << endl;
		return;
	}

	cout << "Would you like to edit the order type? " << endl
		<< "Current order type: " << orderTypeToString(order.orderType) << endl
		<< "Y-yes / N-no: ";
	char command;
	cin >> command;
	clearBuffer();

	if (command == 'Y' || command == 'y')
	{
		inputOrderType(order);
	}

	cout << "Would you like to edit the order status? " << endl
		<< "Current order status: " << orderStatusToString(order.status) << endl
		<< "Y-yes / N-no: ";
	cin >> command;
	clearBuffer();

	if (command == 'Y' || command == 'y')
	{
		inputOrderStatus(order);

		clearBuffer();
		cout << "Input name of technician: ";
		cin.getline(order.technicianName, NAME_LENGTH);
	}
	clearConsole();

	printOrderTableHeader();
	printOrderRow(order);
	orders[index] = order;

	rewriteOrdersInFile(orders);
}
void addService()
{
	Service services[SERVICE_COUNT];
	readServicesFromFile(services);
	int serviceCount = getServiceCount(services);
	if (serviceCount >= SERVICE_COUNT)
	{
		cout << "The number of services in the system is already at the maximum. You cannot add more services." << endl;
		return;
	}
	Service service;
	clearBuffer();
	cout << "Input service type: ";
	cin.getline(service.serviceTypeName, SERVICE_TYPE_NAME);

	while (true)
	{
		cout << "Input standard price for service: ";
		cin >> service.priceForService;
		if (cin.fail())
		{
			cout << "Invalid input! Please try again!" << endl;
			clearBuffer();
			continue;
		}
		break;
	}
	clearBuffer();
	service.serviceNumber = serviceCount + 1;
	services[serviceCount] = service;
	rewriteServicesInFile(services);
}

// Specialised inputs
void inputOrderType(Order& order)
{
	int command = 0;
	bool exit = false;
	while (!exit)
	{
		cout << "= Order Types - Service duration - Price =" << endl;
		cout << "	0. EXPRESS - 2 days - total price is 50% more for service" << endl;
		cout << "	1. FAST - 5 days - total price is 20% more for service" << endl;
		cout << "	2. STANDARD - 10 days - standard price for service" << endl;
		cout << "Input order type: ";
		cin >> command;
		if (cin.fail())
		{
			cout << "Invalid input! Please try again!" << endl;
			clearBuffer();
			continue;
		}
		clearBuffer();
		if (command >= 0 && command <= 2)
		{
			order.orderType = (OrderType)command;
			exit = true;
			switch (command)
			{
			case EXPRESS:
				order.service.daysOfService = 2;
				order.service.priceIncrease = 1.5;
				break;
			case FAST:
				order.service.daysOfService = 5;
				order.service.priceIncrease = 1.2;
				break;
			case STANDARD:
				order.service.daysOfService = 10;
				order.service.priceIncrease = 1;
				break;
			default:
				break;
			}
		}
		else
		{
			cout << "Invalid order type! Please try again!" << endl;
		}
	}
}
void inputOrderStatus(Order& order)
{
	int command = 0;
	bool exit = false;
	while (!exit)
	{
		cout << "= ORDER STATUS =" << endl;
		cout << "	0. ACCEPTED" << endl;
		cout << "	1. REJECTED" << endl;
		cout << "	2. COMPLETED" << endl;
		cout << "Input order status: ";
		cin >> command;
		if (cin.fail())
		{
			cout << "Invalid input! Please try again!" << endl;
			clearBuffer();
			continue;
		}
		clearBuffer();
		if (command >= 0 && command <= 2)
		{
			order.status = (OrderStatus)command;
			exit = true;
		}
		else
		{
			cout << "Invalid order type! Please try again!" << endl;
		}
	}

	if (order.status == COMPLETED)
	{
		double priceIncrease = order.service.priceIncrease;
		int daysOfService = order.service.daysOfService;
		Service service;
		int command = 0;
		bool exit = false;
		while (!exit)
		{
			printAllServices();
			cout << "Choose one service by service number: ";
			cin >> command;
			if (cin.fail())
			{
				cout << "Invalid input! Please try again!" << endl;
				clearBuffer;
				continue;
			}
			if (command < 1 || command > SERVICE_COUNT)
			{
				cout << "Invalid number! Please try again!" << endl;
			}
			service = getServiceByServiceNumber(command);
			if (service.serviceNumber == -1)
			{
				cout << "There was no service found with this service number. Please try again!" << endl;
				continue;
			}
			exit = true;
		}
		service.priceIncrease = priceIncrease;
		service.daysOfService = daysOfService;
		order.service = service;
		order.totalPrice = order.service.priceForService * order.service.priceIncrease;
	}
}

// Additional
int getOrderCount(Order orders[])
{
	for (int i = 0; i < ORDER_COUNT; i++)
	{
		if (orders[i].orderNumber == -1)
		{
			return i;
		}
	}
	return 0;
}
int getServiceCount(Service services[])
{
	for (int i = 0; i < SERVICE_COUNT; i++)
	{
		if (services[i].serviceNumber == -1)
		{
			return i;
		}
	}
	return 0;
}
Service getServiceByServiceNumber(int serviceNumber)
{
	Service services[SERVICE_COUNT];
	readServicesFromFile(services);

	return services[serviceNumber - 1];
}
void clearBuffer()
{
	cin.clear();
	cin.ignore(1000, '\n');
}

// File mamagement
void createFileIfNotExist()
{
	fstream stream;
	stream.open(ORDER_FILE_NAME, ios::binary | ios::in);
	if (stream.fail())
	{
		stream.open(ORDER_FILE_NAME, ios::binary | ios::out);
		stream.close();
	}
	stream.close();

	stream.open(SERVICE_FILE_NAME, ios::binary | ios::in);
	if (stream.fail())
	{
		stream.open(SERVICE_FILE_NAME, ios::binary | ios::out);
		stream.close();
	}
	stream.close();
}
void readOrdersFromFile(Order orders[])
{
	fstream stream;
	stream.open(ORDER_FILE_NAME, ios::binary | ios::in);
	if (stream.fail())
	{
		cout << "File could not be opened!" << endl;
		return;
	}

	stream.seekg(0, ios::end);
	int size = stream.tellg();
	stream.seekg(0, ios::beg);

	int count = size / sizeof(Order);
	stream.read((char*)orders, size);
	stream.close();
}
void rewriteOrdersInFile(Order orders[])
{
	fstream stream;
	stream.open(ORDER_FILE_NAME, ios::binary | ios::out);
	if (stream.fail())
	{
		cout << "File could not be opened!" << endl;
		return;
	}

	int size = ORDER_COUNT * sizeof(Order);
	stream.write((char*)orders, size);
	stream.close();

}
void readServicesFromFile(Service services[])
{
	fstream stream;
	stream.open(SERVICE_FILE_NAME, ios::binary | ios::in);
	if (stream.fail())
	{
		cout << "File could not be opened!" << endl;
		return;
	}

	stream.seekg(0, ios::end);
	int size = stream.tellg();
	stream.seekg(0, ios::beg);

	int count = size / sizeof(Service);
	stream.read((char*)services, size);
	stream.close();
}
void rewriteServicesInFile(Service services[])
{
	fstream stream;
	stream.open(SERVICE_FILE_NAME, ios::binary | ios::out);
	if (stream.fail())
	{
		cout << "File could not be opened!" << endl;
		return;
	}

	int size = SERVICE_COUNT * sizeof(Service);
	stream.write((char*)services, size);
	stream.close();
}

// Enum converters and string manipulations
char* deviceTypeToString(DeviceType deviceType)
{
	switch (deviceType)
	{
	case LAPTOP:
		return (char*)"LAPTOP";
	case PC:
		return (char*)"PC";
	case SMARTPHONE:
		return (char*)"SMARTPHONE";
	case TABLET:
		return (char*)"TABLET";
	case PRINTER:
		return (char*)"PRINTER";
	default:
		return (char*)"UNKNOWN";
	}
}
char* orderTypeToString(OrderType orderType)
{
	switch (orderType)
	{
	case EXPRESS:
		return (char*)"EXPRESS";
	case FAST:
		return (char*)"FAST";
	case STANDARD:
		return (char*)"STANDARD";
	default:
		return (char*)"UNKNOWN";
	}
}
char* orderStatusToString(OrderStatus orderStatus)
{
	switch (orderStatus)
	{
	case ACCEPTED:
		return (char*)"ACCEPTED";
	case REJECTED:
		return (char*)"REJECTED";
	case COMPLETED:
		return (char*)"COMPLETED";
	default:
		return (char*)"UNKNOWN";
	}
}
string truncate(char* str, int width)
{
	string buffer;
	buffer = str;

	if (buffer.length() > width)
	{
		return (buffer.substr(0, width - 3) + "...").c_str();
	}
	return buffer.c_str();
}