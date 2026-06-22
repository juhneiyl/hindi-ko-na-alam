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
using namespace std;

string getTime() {
    time_t now = time(0);
    tm* timeinfo = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%H:%M:%S", timeinfo);
    return string(buf);
}

//singleton
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
        cout << "[" << getTime() << "] [LOG] " << msg << endl;
    }
};

Logger* Logger::instance = NULL;

// base class
class Identifiable {
protected:
    int id;
    string name;

public:
    Identifiable() {
        id = 0;
        name = "UNKNOWN";
    }

    Identifiable(int id, string name) {
        this->id = id;
        this->name = name;
    }

    int getId() {
        return id;
    }

    string getName() {
        return name;
    }

    void setName(string newName) {
        if(newName != "") {
            name = newName;
        }
    }

    virtual ~Identifiable() {}
};

class Observer {
public:
    virtual void update(string eventType) = 0;
    virtual ~Observer() {}
};

class Device : public Observer, public Identifiable {
public:
    Device() : Identifiable() {}
    Device(int id, string name) : Identifiable(id, name) {}

    virtual string getStatus() = 0;
    virtual void update(string eventType) = 0;

    virtual ~Device() {}
};

class Maintainable {
public:
    virtual void logMaintenance() = 0;
    virtual ~Maintainable() {}
};

// this class inherits from both Device and Maintainable.
class SmartDevice : public Device, public Maintainable {
protected:
    string lastMaintenance;

public:
    SmartDevice() : Device() {
        lastMaintenance = "Never";
    }

    SmartDevice(int id, string name) : Device(id, name) {
        lastMaintenance = "Never";
    }

    void logMaintenance() {
        lastMaintenance = "Checked at " + getTime();
        Logger::getInstance()->log("Maintenance logged for " + name);
    }

    string getLastMaintenance() {
        return lastMaintenance;
    }

    virtual ~SmartDevice() {}
};

class TrafficLight : public SmartDevice {
private:
    string mode;

public:
    TrafficLight(int id, string name) : SmartDevice(id, name) {
        mode = "GREEN";
    }

    void update(string eventType) {
        if(eventType == "FIRE" || eventType == "CRITICAL" || eventType == "MOTION") {
            mode = "RED-FLASH";
            cout << "  [DEVICE] " << name << " switched to RED-FLASH mode" << endl;
            Logger::getInstance()->log(name + " switched to RED-FLASH");
        }
    }

    string getStatus() {
        return name + " [Light] Mode: " + mode;
    }

    ~TrafficLight() {}
};

class Siren : public SmartDevice {
private:
    bool active;

public:
    Siren(int id, string name) : SmartDevice(id, name) {
        active = false;
    }

    void update(string eventType) {
        if(eventType == "FIRE" || eventType == "CRITICAL" || eventType == "MOTION") {
            active = true;
            cout << "  [DEVICE] " << name << " is sounding alarm!" << endl;
            Logger::getInstance()->log(name + " activated");
        }
    }

    string getStatus() {
        return name + " [Siren] Status: " + (active ? "ACTIVE" : "IDLE");
    }

    ~Siren() {}
};

class Sensor : public Identifiable {
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

class PowerSensor : public Sensor {
public:
    PowerSensor(int id, string name, string location, double threshold)
        : Sensor(id, name, location, threshold) {}

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

// this is an example of hybrid inheritance. it inherits from PowerSensor
// (which already inherits from Sensor), this creates a chain of inheritance.
class SmartPowerSensor : public PowerSensor {
private:
    bool hasBatteryBackup;
    double batteryLevel;

public:
    SmartPowerSensor(int id, string name, string location, double threshold)
        : PowerSensor(id, name, location, threshold) {
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

class FireSensor : public Sensor {
public:
    FireSensor(int id, string name, string location, double threshold)
        : Sensor(id, name, location, threshold) {}

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

class TemperatureSensor : public Sensor {
public:
    TemperatureSensor(int id, string name, string location, double threshold)
        : Sensor(id, name, location, threshold) {}

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

class MotionSensor : public Sensor {
public:
    MotionSensor(int id, string name, string location, double threshold)
        : Sensor(id, name, location, threshold) {}

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

struct Event {
    int eventId;
    string type;
    string source;
    string timestamp;
};

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
        cout << "[EVENT] " << source << " triggered " << type << " event" << endl;
        Logger::getInstance()->log("Event queued from " + source);
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

class CentralHub {
private:
    static CentralHub* instance;
    vector<Observer*> devices;
    int totalDevices;
    int totalSensors;

    CentralHub() {
        totalDevices = 0;
        totalSensors = 0;
        Logger::getInstance()->log("Central Hub initialized");
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
        cout << "[OBSERVER] " << name << " registered to hub" << endl;
        Logger::getInstance()->log(name + " registered as observer");
    }

    void unregisterDevice(Observer* device) {
        for(int i = 0; i < devices.size(); i++) {
            if(devices[i] == device) {
                devices.erase(devices.begin() + i);
                break;
            }
        }
    }

    // When an event happens, tell all devices
    void notifyDevices(string eventType) {
        cout << "[SYSTEM] Hub notifying all devices..." << endl;
        for(int i = 0; i < devices.size(); i++) {
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

class ControlCenter {
private:
    EventQueue* queue;

public:
    ControlCenter(EventQueue* eq) {
        queue = eq;
    }

    void processEvents(class CentralHub* hub) {
        if(!queue->hasEvents()) {
            return;
        }

        while(queue->hasEvents()) {
            Event e = queue->getNextEvent();
            cout << "\n[CONTROL] Processing event #" << e.eventId << ": " << e.type << endl;
            cout << "[CONTROL] Source: " << e.source << " at " << e.timestamp << endl;
            hub->notifyDevices(e.type);
        }
    }

    ~ControlCenter() {}
};

//from last activity
class DeviceFactory {
public:
    static Device* createDevice(string type, int id, string name) {
        Device* device = NULL;

        if(type == "light") {
            device = new TrafficLight(id, name);
            cout << "[FACTORY] Created TrafficLight: " << name << endl;
        }
        else if(type == "siren") {
            device = new Siren(id, name);
            cout << "[FACTORY] Created Siren: " << name << endl;
        }
        else {
            cout << "[FACTORY] ERROR: Unknown device type" << endl;
        }

        return device;
    }
};

int main() {
    cout << "\n" << string(60, '=') << endl;
    cout << " METRO-GRID RESILIENCE SYSTEM" << endl;
    cout << " Group Number: [YOUR GROUP NUMBER]" << endl;
    cout << string(60, '=') << endl << endl;

    Logger::getInstance()->log("System started");

    // setup
    CentralHub* hub = CentralHub::getInstance();
    EventQueue eventQueue;
    ControlCenter controlCenter(&eventQueue);

    vector<Device*> devices;
    vector<Sensor*> sensors;

    int nextDeviceId = 101;
    int nextSensorId = 501;

    // create initial devices
    cout << "[SETUP] Creating initial devices..." << endl;
    Device* light = DeviceFactory::createDevice("light", nextDeviceId++, "Main Traffic Light");
    Device* siren = DeviceFactory::createDevice("siren", nextDeviceId++, "Emergency Siren");

    devices.push_back(light);
    devices.push_back(siren);

    hub->registerDevice(light, "Main Traffic Light");
    hub->registerDevice(siren, "Emergency Siren");

    // create sensors
    cout << "[SETUP] Creating sensors..." << endl;
    sensors.push_back(new PowerSensor(nextSensorId++, "Power Sensor - Zone A", "Zone A", 220.0));
    sensors.push_back(new SmartPowerSensor(nextSensorId++, "Smart Power Sensor - Zone B", "Zone B", 230.0));
    sensors.push_back(new FireSensor(nextSensorId++, "Fire Sensor - Building 1", "Building 1", 50.0));
    sensors.push_back(new TemperatureSensor(nextSensorId++, "Temperature Sensor - Zone C", "Zone C", 35.0));
    sensors.push_back(new MotionSensor(nextSensorId++, "Motion Sensor - Perimeter", "Perimeter", 5.0));

    hub->setDeviceCount(devices.size());
    hub->setSensorCount(sensors.size());

    cout << "\n[SYSTEM] System ready: " << devices.size() << " devices, "
         << sensors.size() << " sensors\n" << endl;

    // demo: trigger an event
    cout << "[DEMO] Simulating power surge in Zone A..." << endl;
    sensors[0]->setReading(250.0);
    if(sensors[0]->isTriggered()) {
        eventQueue.addEvent(sensors[0]->getEventType(), sensors[0]->getName());
        cout << "\n";
        controlCenter.processEvents(hub);
        cout << endl;
    }

    int choice = 0;
    while(choice != 8) {
        cout << "\n" << string(50, '-') << endl;
        cout << "  MENU" << endl;
        cout << string(50, '-') << endl;
        cout << "1. View all devices" << endl;
        cout << "2. View all sensors" << endl;
        cout << "3. Add new device" << endl;
        cout << "4. Update sensor threshold" << endl;
        cout << "5. Remove device" << endl;
        cout << "6. Simulate sensor reading" << endl;
        cout << "7. System info" << endl;
        cout << "8. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        if(cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "[ERROR] Invalid input\n" << endl;
            continue;
        }

        switch(choice) {
            case 1: {
                // VIEW devices
                cout << "\n[DEVICES LIST]" << endl;
                if(devices.size() == 0) {
                    cout << "No devices active\n" << endl;
                } else {
                    for(int i = 0; i < devices.size(); i++) {
                        SmartDevice* sd = dynamic_cast<SmartDevice*>(devices[i]);
                        if(sd) {
                            cout << "  ID: " << sd->getId() << " - " << sd->getStatus() << endl;
                            cout << "    Last maintenance: " << sd->getLastMaintenance() << endl;
                        }
                    }
                    cout << endl;
                }
                break;
            }

            case 2: {
                // VIEW sensors
                cout << "\n[SENSORS LIST]" << endl;
                if(sensors.size() == 0) {
                    cout << "No sensors active\n" << endl;
                } else {
                    for(int i = 0; i < sensors.size(); i++) {
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
                // CREATE device
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
                // UPDATE sensor
                if(sensors.size() == 0) {
                    cout << "[ERROR] No sensors to update\n" << endl;
                    break;
                }

                cout << "\nEnter sensor ID: ";
                int sensorId;
                cin >> sensorId;

                bool found = false;
                for(int i = 0; i < sensors.size(); i++) {
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
                // DELETE device
                cout << "\nEnter device ID to remove: ";
                int deviceId;
                cin >> deviceId;

                bool found = false;
                for(int i = 0; i < devices.size(); i++) {
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
                // SIMULATE sensor
                if(sensors.size() == 0) {
                    cout << "[ERROR] No sensors available\n" << endl;
                    break;
                }

                cout << "\nEnter sensor ID: ";
                int sensorId;
                cin >> sensorId;

                bool found = false;
                for(int i = 0; i < sensors.size(); i++) {
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
                // STATISTICS
                cout << "\n[SYSTEM STATISTICS]" << endl;
                cout << string(40, '-') << endl;
                cout << "Total Devices:    " << devices.size() << endl;
                cout << "Total Sensors:    " << sensors.size() << endl;
                cout << "Active Observers: " << hub->getObserverCount() << endl;
                cout << "Events Processed: " << eventQueue.getTotalProcessed() << endl;
                cout << "Pending Events:   " << eventQueue.getPendingCount() << endl;
                cout << string(40, '-') << endl << endl;
                break;
            }

            case 8: {
                cout << "\n[SYSTEM] Shutting down...\n" << endl;
                Logger::getInstance()->log("System shutdown");
                break;
            }

            default: {
                cout << "[ERROR] Invalid choice\n" << endl;
            }
        }
    }

    // cleanup
    for(int i = 0; i < devices.size(); i++) {
        delete devices[i];
    }
    for(int i = 0; i < sensors.size(); i++) {
        delete sensors[i];
    }

    cout << endl;
    return 0;
}
