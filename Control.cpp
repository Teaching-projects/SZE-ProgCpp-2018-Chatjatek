#include "Control.hpp"






void Control::showIPList(string errMSG, vector<string> *container) const
{
	if (comData.getIPList().size() == 0) {
		cout << errMSG << endl;
		return;
	}

	for (string IP : comData.getIPList()) {
		cout << "1.. " << IP << endl;
		if (container != nullptr)
			container->push_back(IP);

	}
}

bool Control::chekIfClientIsActive(string IP) const
{
	const vector<Client> &clients = manager.getClients();

	for (const Client &client : clients) {
		if (client.getConnectionIP() == IP) {
			return client.IsActive();
		}
	}

	return false;
}

void Control::showClientErr(const Client &client) const
{
	io.screenShifter(50);

	const deque<string> & errs = client.getErrs();

	cout << client.getClientData() << ": Error list: " << endl;

	for (auto it = errs.crbegin(); it != errs.crend(); ++it) {
		std::cout << *it << endl;
	}
}

Control::Control(string name) : _winsock(), comData(), server(), manager(server, comData), udp(comData)
{
	Client::ownerName = name;

	server.Listen();

	acceptConThread = thread(&Server::AcceptCon, &server);
}


Control::~Control()
{
	manager.ShutDownConnections();
	server.ShutDownConnection();

	this_thread::sleep_for(1s);

	try {
		acceptConThread.join();
	}
	catch(exception)
	{}
}

IOHandling & Control::getIO()
{
	return io;
}

void Control::udpBroadCast()
{
	comData.clearIPList();

	if (!udp.isBroadcast()) {
		udp.refreshBroadcast();
		if (!udp.isBroadcast()) {

			do {
				//bekérni usertõl ip-t vagy broadcastot... és ellenõrizni h helyes-e, + biztositani kilépési lehetõséget ha mégse akarja
			} while (true);
		}
	}

	io.screenShifter(50);
	cout << "Udp felderites, peer-ek keresese!" << endl;
	cout << "Broadcast address: " << udp.getBroadCast() << endl;
	udp.sendTo();

	for (int i = 0; i < 3; i++) {
		cout << "." << endl;
		std::this_thread::sleep_until(system_clock::now() + seconds(1));
	}

	showIPList("Sikertelen volt a feldereites nem talalt peer-t!");

	io.waitToUserInteraction();
}

void Control::showIPs() const
{
	showIPList("Nem talalt peer-t a legutobbi felderites, probalkozzon ujra a 2. menupontal!");

	io.waitToUserInteraction();
}

void Control::connectToPeer()
{
	string IP;
	if (comData.getIPList().size() == 0) {
		cout << "Adjon meg manualisan IP-t vagy vegezzen halozati felderitest (2es menupont)" << endl;
		cout << "Visszateres fomenube a '0'-al" << endl;

		do {
			cin >> IP;

			if (IP.length() == 1 && IP[0] == '0')
				return;
		} while (!io.correctInputIP(IP));
	}
	else{
		cout << "Lista a mar megtalalt peer-ekrol!" << endl;
		cout << "Azonnali kilepeshez irjon be 0-t" << endl;

		vector<string> list;
		showIPList("", &list);

		string in;
		int selected = 0;

		do {
			cin >> in;

			if (in.length() == 1 && in[0] == '0')
				return;
		} while (!io.correctInputDigit(in, selected, 1, list.size()));		

		IP = list[selected - 1];
	}

	if (manager.connectToPeer(IP))
		cout << "Sikeres kapcsolodas!" << endl;
	else
		cout << "Sikertelen kapcsolodas!" << endl;

	io.waitToUserInteraction();
}

void Control::showChat()
{
	io.screenShifter(50);
	cout << "Chateles a peerekkel!" << endl;
	const vector<ClientMessages> &list = comData.getClientList();
	if (list.size() != 0) {
		cout << "Visszateres fomenube a '0'-al" << endl;
		cout << "Aktiv peer lista: " << endl;

		vector<string> actives;
		vector<string> inactives;
		int counter = 0;
		for (ClientMessages client : list) {
			if (chekIfClientIsActive(client.getIP())) {
				cout << ++counter << ".. " << manager.getClient(client.getIP()).getClientData() << endl;	
				actives.push_back(client.getIP());
			}
			else {
				inactives.push_back(client.getIP());
			}
		}
		if (actives.size() == 0)
			cout << "Nincs aktiv peer!" << endl;

		int sizeInAct = inactives.size();
		if (sizeInAct > 0) {
			cout << "Inaktiv peer lista: " << endl;
			for (; counter < sizeInAct; counter++) {
				cout << (counter + 1) << ".. " << manager.getClient(inactives[counter]).getClientData() << endl;
			}
		}

		string in;
		int selected = 0;

		do {
			cin >> in;

			if (in.length() == 1 && in[0] == '0')
				return;
		} while (!io.correctInputDigit(in, selected, 1, counter));


		int sizeAct = actives.size();
		if (selected > sizeAct) {
			const Client & client = manager.getClient(inactives[selected - sizeAct - 1]);

			showClientErr(client);
		}
		else {
			const ClientMessages &client = actives[selected - 1];

			const deque<string> &messages = comData.getClientMessages(client.getIP());

			while(true) {
				io.screenShifter(50);
				cout << "Chateles :" << client.getIP() << endl;
				cout << "Chat frissitese: '00' vel" << endl;
				cout << "kilepes chatbol: '0' vel" << endl << endl;
				for (string s : messages) {
					cout << s << endl;
				}


				cin >> in;

				if (in.length() == 1 && in[0] == '0')
					return;
				if (in.length() == 2) {
					if (in[0] == '0' && in[1] == '0') {
						continue;
					}
				}

				manager.sendMessageTo(in, client.getIP());
			}
		}

	}
	else {
		cout << "Nincs elerheto peer, probalkozzon mas menuponttal!" << endl;
	}

	io.waitToUserInteraction();
}

void Control::disconnectionFrom()
{
	io.screenShifter(50);

	const vector<Client> &clients = manager.getClients();

	if (clients.size() > 0) {

		int counter = 0;
		for (const Client &client : clients) {
			cout << ++counter << ".. " << client.getClientData() << endl;
		}

		cout << ++counter << ".. " << "All" << endl;

		int selected = 0;
		string in;

		do {
			cin >> in;

			if (in.length() == 1 && in[0] == '0')
				return;
		} while (!io.correctInputDigit(in, selected, 1, counter));

		if (selected == counter)
			manager.ShutDownConnections();
		else
			manager.ShutDownConnection(clients[selected - 1].getConnectionIP());

	}
	else
		cout << "Egyetlen peer-el sem all kapcsolatban. Probalkozzon halozati felderitessel (2-es menupont)!" << endl;

	io.waitToUserInteraction();
}

void Control::peerErrs()
{
	io.screenShifter(50);

	cout << "Valsszon kapcsolatot, melnyek megtekintene az utolso 10 hiba uzenetet, '0'al azonnal vissza terhet a fomenube!" << endl;

	const vector<Client> &clients = manager.getClients();

	if (clients.size() > 0) {

		int counter = 0;
		for (const Client &client : clients) {
			cout << ++counter << ".. " << client.getClientData() << endl;
		}

		int selected = 0;
		string in;

		do {
			cin >> in;

			if (in.length() == 1 && in[0] == '0')
				return;
		} while (!io.correctInputDigit(in, selected, 1, counter));

		const Client & client = manager.getClient(clients[selected - 1].getConnectionIP());

		showClientErr(client);
		
	}
	else
		cout << "Egyetlen peer-el sem all kapcsolatban. Probalkozzon halozati felderitessel (2-es menupont)!" << endl;

	io.waitToUserInteraction();
}

void Control::deletePeer()
{
	io.screenShifter(50);

	cout << "Valsszon kapcsolatot, melyet torolni szeretne, '0'al azonnal vissza terhet a fomenube!" << endl;

	const vector<Client> &clients = manager.getClients();

	if (clients.size() > 0) {

		int counter = 0;
		for (const Client &client : clients) {
			cout << ++counter << ".. " << client.getClientData() << endl;
		}

		cout << ++counter << ".. " << "All" << endl;

		int selected = 0;
		string in;

		do {
			cin >> in;

			if (in.length() == 1 && in[0] == '0')
				return;
		} while (!io.correctInputDigit(in, selected, 1, counter));

		io.screenShifter(50);

		if(selected == counter)
		{ 
			manager.deleteClients();

			comData.removeAll();
		}
		else
		{
			string IP = clients[selected - 1].getConnectionIP();

			if (manager.deleteClient(IP))
			{
				if (comData.removeClient(IP)) {
					cout << "Sikeres torles!" << endl;

					io.waitToUserInteraction();
					return;
				}
			}

			cout << "Nem sikerult a torles..." << endl;
		}

	}
	else
		cout << "Egyetlen peer-el sem all kapcsolatban. Probalkozzon halozati felderitessel (2-es menupont)!" << endl;

	io.waitToUserInteraction();
}
