/*****************************************************************************************************************
* Group Number: 4
* Group Members: Janelle Rhian Barrera, Humphrey Barcon, Koby Bongat, Alexander Paul Jimenez, John Paul Macario
* Final Project/Exam: The Integrated Engineering System
*****************************************************************************************************************/

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>
using namespace std;

string getTime() {
	time_t now = time(0);
	tm* timeinfo = localtime(&now);
	char buf[20];
	strftime(buf, sizeof(buf), "%H:%M:%S", timeinfo);
	return string(buf);
}

// singleton
class Logger {
private:
	static Logger* instance;
	Logger() {} 

public:
	static Logger* getInstance() {
		if(instance == NULL) {
			instance = new Logger();
		}
		return instance;
	}

	void log(string msg) {
		cout << " | [" << getTime() << "] [LOG] " << msg << endl;
	}
};

Logger* Logger::instance = NULL;

// base class
class Identifiable {
private:
	int id;      
	string name;  

public:
	Identifiable() {
		id = 0;
		name = "UNKNOWN";
	}

	Identifiable(int id, string name) {
		setId(id);
		setName(name);
	}

	int getId() {
		return id;
	}

	string getName() {
		return name;
	}

	void setId(int newId) {
		if(newId >= 0) {  
			id = newId;
		}
	}

	void setName(string newName) {
		if(newName != "") {  
			name = newName;
		}
	}

	virtual ~Identifiable() {}
};

// abstract base class
class Observer {
public:
	virtual void update(string eventType) = 0;
	virtual ~Observer() {}
};

// abstract base class
class Device : public Observer, virtual public Identifiable {
public:
	Device() : Identifiable() {}
	Device(int id, string name) : Identifiable(id, name) {}

	virtual void update(string eventType) = 0;
	virtual string getStatus() = 0;

	virtual ~Device() {}
};

// abstract base class
class Maintainable {
public:
	virtual void logMaintenance() = 0;
	virtual ~Maintainable() {}
};

class SmartDevice : public Device, public Maintainable {
protected:
	string lastMaintenance;

public:
	SmartDevice() : Device() {
		lastMaintenance = "Never";
	}

	SmartDevice(int id, string name) : Identifiable(id, name), Device(id, name) {
		lastMaintenance = "Never";
	}

	void logMaintenance() {
		lastMaintenance = "Checked at " + getTime();
		Logger::getInstance()->log("Maintenance logged for " + getName());
	}

	string getLastMaintenance() {
		return lastMaintenance;
	}

	virtual ~SmartDevice() {}
};

// concrete class
class TrafficLight : public SmartDevice {
private:
	string mode; 

public:
	TrafficLight(int id, string name) : Identifiable(id, name), SmartDevice(id, name) {
		mode = "GREEN";
	}

	void update(string eventType) {
		if(eventType == "FIRE" || eventType == "CRITICAL" || eventType == "MOTION") {
			mode = "RED-FLASH";
			cout << " | [DEVICE] " << getName() << " switched to RED-FLASH mode              |" << endl;
			Logger::getInstance()->log(getName() + " switched to RED-FLASH           |");
		} 
	}

	string getStatus() {
		return getName() + " [Light] Mode: " + mode;
	}

	~TrafficLight() {}
};

// concrete class
class Siren : public SmartDevice {
private:
	bool active; 

public:
	Siren(int id, string name) : Identifiable(id, name), SmartDevice(id, name) {
		active = false;
	}

	void update(string eventType) {
		if(eventType == "FIRE" || eventType == "CRITICAL" || eventType == "MOTION") {
			active = true;
			cout << " | [DEVICE] " << getName() << " is sounding alarm!                         |" << endl;
			Logger::getInstance()->log(getName() + " activated                          |");
		}
	}

	string getStatus() {
		return getName() + " [Siren] Status: " + (active ? "ACTIVE" : "IDLE");
	}

	~Siren() {}
};

// abstract base class
class Sensor : virtual public Identifiable {
protected:
	string location;   
	double reading;   
	double threshold;

public:
	Sensor() : Identifiable() {
		location = "Unknown";
		reading = 0;
		threshold = 100;
	}

	Sensor(int id, string name, string location, double threshold)
		: Identifiable(id, name) {
		this->location = location;
		this->reading = 0;
		this->threshold = threshold;
	}

	void setReading(double r) {
		reading = r;
	}

	double getReading() {
		return reading;
	}

	void setThreshold(double t) {
		if(t > 0) {  
			threshold = t;
		}
	}

	double getThreshold() {
		return threshold;
	}

	string getLocation() {
		return location;
	}

	virtual bool isTriggered() = 0;
	virtual string getEventType() = 0;
	virtual string getSensorType() = 0;

	virtual ~Sensor() {}
};

// concrete class 
class PowerSensor : public Sensor {
public:
	PowerSensor(int id, string name, string location, double threshold)
		: Identifiable(id, name), Sensor(id, name, location, threshold) {}

	bool isTriggered() {
		return reading > threshold;  
	}

	string getEventType() {
		return "CRITICAL";
	}

	string getSensorType() {
		return "PowerSensor";
	}

	~PowerSensor() {}
};

// concrete class
class SmartPowerSensor : public PowerSensor {
private:
	bool hasBatteryBackup;
	double batteryLevel;

public:
	SmartPowerSensor(int id, string name, string location, double threshold)
		: Identifiable(id, name), PowerSensor(id, name, location, threshold) {
		hasBatteryBackup = true;
		batteryLevel = 100.0;
	}

	void drainBattery(double amount) {
		batteryLevel -= amount;
		if(batteryLevel < 0) batteryLevel = 0;
	}

	double getBatteryLevel() {
		return batteryLevel;
	}

	bool isBatteryLow() {
		return batteryLevel < 20;
	}

	string getSensorType() {
		return "SmartPowerSensor (with backup)";
	}

	~SmartPowerSensor() {}
};

// concrete class 
class FireSensor : public Sensor {
public:
	FireSensor(int id, string name, string location, double threshold)
		: Identifiable(id, name), Sensor(id, name, location, threshold) {}

	bool isTriggered() {
		return reading > threshold;
	}

	string getEventType() {
		return "FIRE";
	}

	string getSensorType() {
		return "FireSensor";
	}

	~FireSensor() {}
};

// concrete class
class TemperatureSensor : public Sensor {
public:
	TemperatureSensor(int id, string name, string location, double threshold)
		: Identifiable(id, name), Sensor(id, name, location, threshold) {}

	bool isTriggered() {
		return reading > threshold;
	}

	string getEventType() {
		return "ALARM";
	}

	string getSensorType() {
		return "TemperatureSensor";
	}

	~TemperatureSensor() {}
};

// concrete class
class MotionSensor : public Sensor {
public:
	MotionSensor(int id, string name, string location, double threshold)
		: Identifiable(id, name), Sensor(id, name, location, threshold) {}

	bool isTriggered() {
		return reading > threshold;
	}

	string getEventType() {
		return "MOTION";
	}

	string getSensorType() {
		return "MotionSensor";
	}

	~MotionSensor() {}
};

// here, we got diamond inheritance class 
// this class inherits from both Device and Sensor.
// because both Device and Sensor use VIRTUAL inheritance
// from Identifiable, IntelligentController receives only ONE
// copy of id and name (not two). this solves the Diamond Problem.

class IntelligentController : public Device, public Sensor {
private:
	bool autoResponseEnabled; 
	int eventsCaught;         
	int alertsTriggered;    
	
public:
	IntelligentController(int id, string name, string location, double threshold)
		: Identifiable(id, name), Device(id, name), Sensor(id, name, location, threshold) {
		autoResponseEnabled = true;
		eventsCaught = 0;
		alertsTriggered = 0;
		
		Logger::getInstance()->log("IntelligentController created.                     |");
	}

	
	// detects if reading exceeds threshold (as a Sensor)
	bool isTriggered() {
		return reading > threshold;
	}

	// returns the type of event if triggered
	string getEventType() {
		return "INTELLIGENT_ALERT";
	}

	string getSensorType() {
		return "IntelligentController (Device+Sensor)";
	}

    // responds to events from the hub (as a Device)
	void update(string eventType) {
		eventsCaught++;
		
		if(autoResponseEnabled) {
			cout << " | [DEVICE] " << getName() << " received event: " << eventType << "             |" << endl;
			
			// smart response based on event type
			if(eventType == "FIRE") {
				cout << " | [RESPONSE] " << getName() << " initiating fire containment...       |" << endl;
				alertsTriggered++;
			} else if(eventType == "CRITICAL") {
				cout << " | [RESPONSE] " << getName() << " load balancing activated...        |" << endl;
				alertsTriggered++;
			} else if(eventType == "MOTION") {
				cout << " | [RESPONSE] " << getName() << " security alert logged...          |" << endl;
				alertsTriggered++;
			}
			
			Logger::getInstance()->log(getName() + " auto-responded to " + eventType + "   |");
		}
	}

	// returns status as a smart device
	string getStatus() {
		return getName() + " [IntelligentController] Auto: " + 
		       (autoResponseEnabled ? "ON" : "OFF") + 
		       " | Events: " + to_string(eventsCaught) + 
		       " | Alerts: " + to_string(alertsTriggered);
	}

	void enableAutoResponse(bool enabled) {
		autoResponseEnabled = enabled;
	}

	bool isAutoResponseEnabled() {
		return autoResponseEnabled;
	}

	int getEventsCaught() {
		return eventsCaught;
	}

	int getAlertsTriggered() {
		return alertsTriggered;
	}

	~IntelligentController() {}
};

struct Event {
	int eventId;
	string type;
	string source;
	string timestamp;
};

// Sensors push events to a queue instead of calling devices directly
// ControlCenter processes the queue and tells CentralHub to notify observer devices
class EventQueue {
private:
	queue<Event> events;
	int eventCounter;
	int totalProcessed;

public:
	EventQueue() {
		eventCounter = 1;
		totalProcessed = 0;
	}

	void addEvent(string type, string source) {
		Event e;
		e.eventId = eventCounter++;
		e.type = type;
		e.source = source;
		e.timestamp = getTime();
		events.push(e);
		cout << " | [EVENT] " << source << " triggered " << type << " event              |" << endl;
		Logger::getInstance()->log("Event queued from " + source + "            |");
	}

	bool hasEvents() {
		return !events.empty();
	}

	int getPendingCount() {
		return events.size();
	}

	Event getNextEvent() {
		Event e = events.front();
		events.pop();
		totalProcessed++;
		return e;
	}

	int getTotalProcessed() {
		return totalProcessed;
	}

	~EventQueue() {}
};

// forward declaration
class CentralHub;

// processes events from the queue and
// dispatches them to the CentralHub for device notification.
class ControlCenter {
private:
	EventQueue* queue;

public:
	ControlCenter(EventQueue* eq) {
		queue = eq;
	}

	void processEvents(CentralHub* hub); 

	~ControlCenter() {}
};

// Singleton and Observer: CentralHub is the single subject that notifies all observer devices
// Polymorphism: CentralHub calls each device's update() method, triggering distinct behaviors
class CentralHub {
private:
	static CentralHub* instance;
	vector<Observer*> devices; 
	int totalDevices;
	int totalSensors;

	CentralHub() {
		totalDevices = 0;
		totalSensors = 0;
		Logger::getInstance()->log("Central Hub initialized                            |");
        cout << " |                                                                     |"<<endl;
	}

public:
	static CentralHub* getInstance() {
		if(instance == NULL) {
			instance = new CentralHub();
		}
		return instance;
	}

	void registerDevice(Observer* device, string name) {
    devices.push_back(device);
    
    cout << " | " << left << setw(66) << (name + " registered to hub") << "  |" << endl;
    cout << " |                                                                     |" << endl;
    
    ostringstream logStream;
    logStream << " " << left << setw(49) << (name + " registered as observer") << " |";
    
    Logger::getInstance()->log(logStream.str());
    cout << " |                                                                     |" << endl;
	}

	void unregisterDevice(Observer* device) {
		for(int i = 0; i < (int)devices.size(); i++) {
			if(devices[i] == device) {
				devices.erase(devices.begin() + i);
				break;
			}
		}
	}

	void notifyDevices(string eventType) {
		cout << " | [SYSTEM] Hub notifying all devices...                               |" << endl;
		for(int i = 0; i < (int)devices.size(); i++) {
			devices[i]->update(eventType); 
		}
	}

	int getDeviceCount() {
		return totalDevices;
	}

	int getSensorCount() {
		return totalSensors;
	}

	int getObserverCount() {
		return devices.size();
	}

	void setDeviceCount(int count) {
		totalDevices = count;
	}

	void setSensorCount(int count) {
		totalSensors = count;
	}

	~CentralHub() {}
};

CentralHub* CentralHub::instance = NULL;

// now that CentralHub is fully defined
inline void ControlCenter::processEvents(CentralHub* hub) {
	if(!queue->hasEvents()) {
		return;
	}

	while(queue->hasEvents()) {
		Event e = queue->getNextEvent();
		cout << " | [CONTROL] Processing event #" << e.eventId << ": " << e.type <<  "                             |"<<endl;
		cout << " | [CONTROL] Source: " << e.source << " at " << e.timestamp << "                 |"<<endl;
		hub->notifyDevices(e.type);
	}
}

class DeviceFactory {
public:
	static Device* createDevice(string type, int id, string name) {
		Device* device = NULL;

		if(type == "light") {
			device = new TrafficLight(id, name);
			cout << " | [FACTORY] Created TrafficLight: " << name << "                  |"<<endl;
		}
		else if(type == "siren") {
			device = new Siren(id, name);
			cout << " | [FACTORY] Created Siren: " << name << "                            |"<<endl;
			cout << " |                                                                     |"<<endl;
		}
		else {
			cout << " | [FACTORY] ERROR: Unknown device type" << endl;
			cout << " |                                                                     |"<<endl;
		}

		return device;
	}
};

int main() {
	cout << " -----------------------------------------------------------------------" << endl;
	cout << " |    Group 4: Final Project/Exam: The Integrated Engineering System   |" << endl;
	cout << " -----------------------------------------------------------------------"<< endl;

	Logger::getInstance()->log("System started                                     |");

	CentralHub* hub = CentralHub::getInstance();
	EventQueue eventQueue;
	ControlCenter controlCenter(&eventQueue);

	vector<Device*> devices;
	vector<Sensor*> sensors;
	
	IntelligentController* smartController = NULL;

	int nextDeviceId = 101;
	int nextSensorId = 501;

	cout << " | [SETUP] Creating initial devices...                                 |" << endl;
	cout << " |                                                                     |"<<endl;

	Device* light = DeviceFactory::createDevice("light", nextDeviceId++, "Main Traffic Light");
	Device* siren = DeviceFactory::createDevice("siren", nextDeviceId++, "Emergency Siren");

	devices.push_back(light);
	devices.push_back(siren);

	hub->registerDevice(light, "Main Traffic Light");
	hub->registerDevice(siren, "Emergency Siren");

	cout << " | [SETUP] Creating sensors...                                         |" << endl;
	sensors.push_back(new PowerSensor(nextSensorId++, "Power Sensor - Zone A", "Zone A", 220.0));
	sensors.push_back(new SmartPowerSensor(nextSensorId++, "Smart Power Sensor - Zone B", "Zone B", 230.0));
	sensors.push_back(new FireSensor(nextSensorId++, "Fire Sensor - Building 1", "Building 1", 50.0));
	sensors.push_back(new TemperatureSensor(nextSensorId++, "Temperature Sensor - Zone C", "Zone C", 35.0));
	sensors.push_back(new MotionSensor(nextSensorId++, "Motion Sensor - Perimeter", "Perimeter", 5.0));

	cout << " | [SETUP] Creating IntelligentController (Diamond Inheritance)...     |" << endl;
	cout << " |                                                                     |" << endl;
	smartController = new IntelligentController(nextDeviceId++, "Smart Grid Controller", "Central Hub", 240.0);
	cout << " |                                                                     |" << endl;
	devices.push_back(smartController);
	sensors.push_back(smartController);
	hub->registerDevice(smartController, "Smart Grid Controller");

	hub->setDeviceCount(devices.size());
	hub->setSensorCount(sensors.size());

	cout << " | [SYSTEM] System ready: " << devices.size() << " devices, "
	     << sensors.size() << " sensors                         |\n";
	cout << " |                                                                     |"<<endl;

	cout << " | [DEMO] Simulating power surge in Zone A...                          |" << endl;
	sensors[0]->setReading(250.0);
	if(sensors[0]->isTriggered()) {
		eventQueue.addEvent(sensors[0]->getEventType(), sensors[0]->getName());
		controlCenter.processEvents(hub);
		cout << " -----------------------------------------------------------------------"<<endl;
	}

	int choice = 0;
	while(choice != 8) {
		cout << " \n -----------------------------------------------------------------------"<<endl;
		cout << " |                                  MENU                               |"<< endl;
		cout << " -----------------------------------------------------------------------"<<endl;
		cout << " | 1. View all devices                                                 |"<< endl;
		cout << " | 2. View all sensors                                                 |" << endl;
		cout << " | 3. Add new device                                                   |" << endl;
		cout << " | 4. Update sensor threshold                                          |" << endl;
		cout << " | 5. Remove device                                                    |" << endl;
		cout << " | 6. Simulate sensor reading                                          |" << endl;
		cout << " | 7. System info                                                      |" << endl;
		cout << " | 8. Exit                                                             |" << endl;
		cout << " -----------------------------------------------------------------------"<<endl;
		cout << " \n Enter choice: ";
		cin >> choice;

		if(cin.fail()) {
			cin.clear();
			cin.ignore(1000, '\n');
			cout << "[ERROR] Invalid input\n" << endl;
			continue;
		}

		switch(choice) {
		case 1: {
			cout << "\n[DEVICES LIST]" << endl;
			if(devices.size() == 0) {
				cout << "No devices active\n" << endl;
			} else {
				for(int i = 0; i < (int)devices.size(); i++) {
					SmartDevice* sd = dynamic_cast<SmartDevice*>(devices[i]);
					IntelligentController* ic = dynamic_cast<IntelligentController*>(devices[i]);
					
					if(ic) {
						cout << "  ID: " << ic->getId() << " - " << ic->getStatus() << endl;
					} else if(sd) {
						cout << "  ID: " << sd->getId() << " - " << sd->getStatus() << endl;
						cout << "    Last maintenance: " << sd->getLastMaintenance() << endl;
					}
				}
				cout << endl;
			}
			break;
		}

		case 2: {
			cout << "\n[SENSORS LIST]" << endl;
			if(sensors.size() == 0) {
				cout << "No sensors active\n" << endl;
			} else {
				for(int i = 0; i < (int)sensors.size(); i++) {
					cout << "  ID: " << sensors[i]->getId() << " [" << sensors[i]->getSensorType() << "]" << endl;
					cout << "    Name: " << sensors[i]->getName() << endl;
					cout << "    Location: " << sensors[i]->getLocation() << endl;
					cout << "    Reading: " << sensors[i]->getReading()
					     << " / Threshold: " << sensors[i]->getThreshold();

					SmartPowerSensor* smartSensor = dynamic_cast<SmartPowerSensor*>(sensors[i]);
					if(smartSensor) {
						cout << " | Battery: " << smartSensor->getBatteryLevel() << "%";
					}
					cout << endl;
				}
				cout << endl;
			}
			break;
		}

		case 3: {
			cout << "\nEnter device type (light/siren): ";
			string type;
			cin >> type;
			cout << "Enter device name: ";
			cin.ignore();
			string name;
			getline(cin, name);

			Device* newDevice = DeviceFactory::createDevice(type, nextDeviceId++, name);
			if(newDevice != NULL) {
				devices.push_back(newDevice);
				hub->registerDevice(newDevice, name);
				hub->setDeviceCount(devices.size());
				cout << "[CREATE] Device added successfully\n" << endl;
			}
			break;
		}

		case 4: {
			if(sensors.size() == 0) {
				cout << "[ERROR] No sensors to update\n" << endl;
				break;
			}

			cout << "\nEnter sensor ID: ";
			int sensorId;
			cin >> sensorId;

			bool found = false;
			for(int i = 0; i < (int)sensors.size(); i++) {
				if(sensors[i]->getId() == sensorId) {
					cout << "Enter new threshold: ";
					double newThreshold;
					cin >> newThreshold;
					sensors[i]->setThreshold(newThreshold);
					cout << "[UPDATE] Sensor threshold updated\n" << endl;
					Logger::getInstance()->log("Sensor threshold updated");
					found = true;
					break;
				}
			}

			if(!found) {
				cout << "[ERROR] Sensor not found\n" << endl;
			}
			break;
		}

		case 5: {
			cout << "\nEnter device ID to remove: ";
			int deviceId;
			cin >> deviceId;

			bool found = false;
			for(int i = 0; i < (int)devices.size(); i++) {
				if(devices[i]->getId() == deviceId) {
					cout << "[DELETE] Device " << devices[i]->getName() << " removed\n" << endl;
					Logger::getInstance()->log("Device removed");
					hub->unregisterDevice(devices[i]);
					delete devices[i];
					devices.erase(devices.begin() + i);
					hub->setDeviceCount(devices.size());
					found = true;
					break;
				}
			}

			if(!found) {
				cout << "[ERROR] Device not found\n" << endl;
			}
			break;
		}

		case 6: {
			if(sensors.size() == 0) {
				cout << "[ERROR] No sensors available\n" << endl;
				break;
			}

			cout << "\nEnter sensor ID: ";
			int sensorId;
			cin >> sensorId;

			bool found = false;
			for(int i = 0; i < (int)sensors.size(); i++) {
				if(sensors[i]->getId() == sensorId) {
					cout << "Enter reading value: ";
					double reading;
					cin >> reading;
					sensors[i]->setReading(reading);
					cout << "[SIMULATION] " << sensors[i]->getName() << " reading: " << reading << endl;

					SmartPowerSensor* smartSensor = dynamic_cast<SmartPowerSensor*>(sensors[i]);
					if(smartSensor) {
						smartSensor->drainBattery(5.0);
						if(smartSensor->isBatteryLow()) {
							cout << "[WARNING] Battery low on " << smartSensor->getName() << endl;
						}
					}

					if(sensors[i]->isTriggered()) {
						cout << "[ALERT] Threshold exceeded!\n" << endl;
						eventQueue.addEvent(sensors[i]->getEventType(), sensors[i]->getName());
						cout << "\n";
						controlCenter.processEvents(hub);
						cout << endl;
					} else {
						cout << "[INFO] Reading is within normal range\n" << endl;
					}
					found = true;
					break;
				}
			}

			if(!found) {
				cout << "[ERROR] Sensor not found\n" << endl;
			}
			break;
		}

		case 7: {
			cout << "\n -----------------------------------------------------------------------"<<endl;
			cout << " |                         [SYSTEM STATISTICS]                         |"<< endl;
		    cout << " -----------------------------------------------------------------------"<<endl;
			cout << " | Total Devices:    " << devices.size() << "                                                 |"<< endl;
			cout << " | Total Sensors:    " << sensors.size() << "                                                 |"<< endl;
			cout << " | Active Observers: " << hub->getObserverCount() << "                                                 |"<< endl;
			cout << " | Events Processed: " << eventQueue.getTotalProcessed() << "                                                 |"<< endl;
			cout << " | Pending Events:   " << eventQueue.getPendingCount() << "                                                 |"<< endl;
			if(smartController) {
				cout << " | Controller Events Caught: " << smartController->getEventsCaught() << "                                         |"<< endl;
				cout << " | Controller Alerts Triggered: " << smartController->getAlertsTriggered() << "                                      |"<< endl;
			}
		    cout << " -----------------------------------------------------------------------"<<endl;
			break;
		}

		case 8: {
			cout << "\n [SYSTEM] Shutting down...\n" << endl;
			break;
		}

		default: {
			cout << " [ERROR] Invalid choice" << endl;
		}
		}
	}

	for(int i = 0; i < (int)devices.size(); i++) {
		delete devices[i];
	}
	
	for(int i = 0; i < (int)sensors.size(); i++) {
		IntelligentController* ic = dynamic_cast<IntelligentController*>(sensors[i]);
		if(!ic) { 
			delete sensors[i];
		}
	}

	cout << endl;
	return 0;
}
