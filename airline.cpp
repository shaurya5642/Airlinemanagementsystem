#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <memory>

using namespace std;

// Flight Class
class Flight {
private:
    int flightNumber;
    string source, destination, date;
    int totalSeats;
    int bookedSeats;

public:
    Flight() {}

    Flight(int no, string src, string dest, string dt, int seats) {
        flightNumber = no;
        source = src;
        destination = dest;
        date = dt;
        totalSeats = seats;
        bookedSeats = 0;
    }

    int getFlightNumber() const { return flightNumber; }
    string getSource() const { return source; }
    string getDestination() const { return destination; }
    string getDate() const { return date; }
    int getTotalSeats() const { return totalSeats; }
    int getBookedSeats() const { return bookedSeats; }
    
    void bookSeat() {
        if(bookedSeats < totalSeats)
            bookedSeats++;
    }

    void cancelSeat() {
        if(bookedSeats > 0)
            bookedSeats--;
    }

    bool seatsAvailable() {
        return bookedSeats < totalSeats;
    }

    string serialize() {
        return to_string(flightNumber) + "|" + source + "|" + destination + "|" + date + "|" + to_string(totalSeats) + "|" + to_string(bookedSeats);
    }

    static Flight deserialize(string line) {
        Flight f;
        stringstream ss(line);
        string temp;
        getline(ss, temp, '|'); f.flightNumber = stoi(temp);
        getline(ss, f.source, '|');
        getline(ss, f.destination, '|');
        getline(ss, f.date, '|');
        getline(ss, temp, '|'); f.totalSeats = stoi(temp);
        getline(ss, temp, '|'); f.bookedSeats = stoi(temp);
        return f;
    }

    void display() {
        cout << "Flight No: " << flightNumber << ", From: " << source << ", To: " << destination
             << ", Date: " << date << ", Seats: " << bookedSeats << "/" << totalSeats << endl;
    }
};

// Passenger Class
class Passenger {
private:
    int ticketNo;
    string name;
    int age;
    int flightNo;
    int seatNo;

public:
    Passenger() {}

    Passenger(int tNo, string nm, int ag, int fNo, int sNo) {
        ticketNo = tNo;
        name = nm;
        age = ag;
        flightNo = fNo;
        seatNo = sNo;
    }

    int getTicketNo() { return ticketNo; }
    int getFlightNo() { return flightNo; }
    int getSeatNo() { return seatNo; }

    string serialize() {
        return to_string(ticketNo) + "|" + name + "|" + to_string(age) + "|" + to_string(flightNo) + "|" + to_string(seatNo);
    }

    static Passenger deserialize(string line) {
        Passenger p;
        stringstream ss(line);
        string temp;
        getline(ss, temp, '|'); p.ticketNo = stoi(temp);
        getline(ss, p.name, '|');
        getline(ss, temp, '|'); p.age = stoi(temp);
        getline(ss, temp, '|'); p.flightNo = stoi(temp);
        getline(ss, temp, '|'); p.seatNo = stoi(temp);
        return p;
    }

    void display() {
        cout << "Ticket No: " << ticketNo << ", Name: " << name << ", Age: " << age
             << ", Flight No: " << flightNo << ", Seat No: " << seatNo << endl;
    }
};

// Abstract SeatMap Class
class SeatMap {
protected:
    vector<bool> seats;
public:
    virtual int findAvailableSeat() = 0;
    virtual void occupy(int index) { seats[index] = true; }
    virtual void release(int index) { seats[index] = false; }
};

// Economy SeatMap
class EconomySeatMap : public SeatMap {
public:
    EconomySeatMap(int count = 50) { seats.resize(count, false); }
    int findAvailableSeat() override {
        for(int i = 0; i < seats.size(); i++) {
            if(!seats[i]) return i;
        }
        return -1;
    }
};

// Business SeatMap
class BusinessSeatMap : public SeatMap {
public:
    BusinessSeatMap(int count = 10) { seats.resize(count, false); }
    int findAvailableSeat() override {
        for(int i = 0; i < seats.size(); i++) {
            if(!seats[i]) return i;
        }
        return -1;
    }
};

// Singleton BookingSystem
class BookingSystem {
private:
    vector<Flight> flights;
    map<int, Passenger> passengers;
    static BookingSystem* instance;
    int ticketCounter;

    BookingSystem() { ticketCounter = 1000; loadAll(); }

    void loadAll() {
        loadFlights();
        loadPassengers();
    }

    void loadFlights() {
        ifstream fin("flightdata.txt");
        string line;
        while(getline(fin, line)) {
            flights.push_back(Flight::deserialize(line));
        }
        fin.close();
    }

    void loadPassengers() {
        ifstream fin("passengerdata.txt");
        string line;
        while(getline(fin, line)) {
            Passenger p = Passenger::deserialize(line);
            passengers[p.getTicketNo()] = p;
        }
        fin.close();
    }

    void saveFlights() {
        ofstream fout("flightdata.txt");
        for(auto &f : flights) {
            fout << f.serialize() << endl;
        }
        fout.close();
    }

    void savePassengers() {
        ofstream fout("passengerdata.txt");
        for(auto &p : passengers) {
            fout << p.second.serialize() << endl;
        }
        fout.close();
    }

public:
    static BookingSystem* getInstance() {
        if(instance == nullptr) instance = new BookingSystem();
        return instance;
    }

    void listFlights() {
        for(auto &f : flights) {
            f.display();
        }
    }

    void addFlight() {
        int no, seats;
        string src, dest, date;
        cout << "Enter Flight No, Source, Destination, Date, Total Seats:\n";
        cin >> no >> src >> dest >> date >> seats;
        Flight f(no, src, dest, date, seats);
        flights.push_back(f);
        saveFlights();
        cout << "Flight Added!\n";
    }

    void bookTicket() {
        int fno;
        string name;
        int age, classType;
        cout << "Enter Flight No: ";
        cin >> fno;
        cout << "Enter Name and Age: ";
        cin >> name >> age;
        cout << "Enter Class (1-Economy, 2-Business): ";
        cin >> classType;

        Flight *flight = nullptr;
        for(auto &f : flights) {
            if(f.getFlightNumber() == fno && f.seatsAvailable()) {
                flight = &f;
                break;
            }
        }

        if(!flight) {
            cout << "Flight not found or Full!\n";
            return;
        }

        SeatMap *sm;
        if(classType == 1) sm = new EconomySeatMap(flight->getTotalSeats());
        else sm = new BusinessSeatMap(flight->getTotalSeats());

        int seat = sm->findAvailableSeat();
        if(seat == -1) {
            cout << "No seats available in selected class!\n";
            delete sm;
            return;
        }

        sm->occupy(seat);
        int tNo = ticketCounter++;
        Passenger p(tNo, name, age, fno, seat);
        passengers[tNo] = p;
        flight->bookSeat();
        saveFlights();
        savePassengers();
        cout << "Ticket Booked! Ticket No: " << tNo << ", Seat No: " << seat << endl;
        delete sm;
    }

    void cancelTicket() {
        int tNo;
        cout << "Enter Ticket No to cancel: ";
        cin >> tNo;

        if(passengers.find(tNo) == passengers.end()) {
            cout << "Ticket not found!\n";
            return;
        }

        Passenger p = passengers[tNo];
        int fNo = p.getFlightNo();
        int seat = p.getSeatNo();

        for(auto &f : flights) {
            if(f.getFlightNumber() == fNo) {
                f.cancelSeat();
                break;
            }
        }
        passengers.erase(tNo);
        saveFlights();
        savePassengers();
        cout << "Ticket Cancelled.\n";
    }
};

BookingSystem* BookingSystem::instance = nullptr;

// Main Menu
int main() {
    BookingSystem* system = BookingSystem::getInstance();
    int choice;

    while(true) {
        cout << "\n1. List Flights\n2. Add Flight (Admin)\n3. Book Ticket\n4. Cancel Ticket\n5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch(choice) {
            case 1: system->listFlights(); break;
            case 2: system->addFlight(); break;
            case 3: system->bookTicket(); break;
            case 4: system->cancelTicket(); break;
            case 5: exit(0);
            default: cout << "Invalid Choice!\n";
        }
    }

    return 0;
}
