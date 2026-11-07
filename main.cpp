#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

class Advisory {
public:
    virtual void giveAdvice() const = 0;
    virtual ~Advisory() {}
};

class Crop : public Advisory {
protected:
    string name;
    double area;
    string season;
    double yieldPerAcre;
    static int cropCount;

public:
    Crop() : name("Unknown"), area(0.0), season("Unknown"), yieldPerAcre(0.0) {
        cropCount++;
    }

    Crop(string n, double a, string s, double y) : name(n), season(s) {
        if (a <= 0) throw invalid_argument("Area must be greater than zero.");
        if (y < 0) throw invalid_argument("Yield per acre cannot be negative.");
        area = a;
        yieldPerAcre = y;
        cropCount++;
    }

    virtual ~Crop() {
        cropCount--;
    }

    static int getCropCount() {
        return cropCount;
    }

    string getName() const { return name; }
    double getArea() const { return area; }
    string getSeason() const { return season; }
    double getYieldPerAcre() const { return yieldPerAcre; }

    virtual double calculateTotalYield() const {
        return area * yieldPerAcre;
    }

    virtual void display() const {
        cout << left << setw(15) << name 
             << setw(10) << area 
             << setw(12) << season 
             << setw(15) << yieldPerAcre;
    }

    bool operator>(const Crop& other) const {
        return this->calculateTotalYield() > other.calculateTotalYield();
    }
};

int Crop::cropCount = 0;

class CerealCrop : public Crop {
private:
    string fertilizerType;

public:
    CerealCrop() : Crop(), fertilizerType("None") {}

    CerealCrop(string n, double a, string s, double y, string f) 
        : Crop(n, a, s, y), fertilizerType(f) {}

    double calculateTotalYield() const override {
        return area * yieldPerAcre; 
    }

    void display() const override {
        Crop::display();
        cout << setw(20) << fertilizerType << setw(15) << "N/A" 
             << setw(15) << calculateTotalYield() << endl;
    }

    void giveAdvice() const override {
        cout << "[Advisory for " << name << "]: Maintain nitrogen levels and monitor water logic during grain filling." << endl;
    }

    string getFertilizerType() const { return fertilizerType; }
};

class VegetableCrop : public Crop {
private:
    int irrigationFrequency;

public:
    VegetableCrop() : Crop(), irrigationFrequency(0) {}

    VegetableCrop(string n, double a, string s, double y, int freq) 
        : Crop(n, a, s, y) {
        if (freq <= 0) throw invalid_argument("Irrigation frequency must be positive.");
        irrigationFrequency = freq;
    }

    double calculateTotalYield() const override {
        return area * yieldPerAcre;
    }

    void display() const override {
        Crop::display();
        cout << setw(20) << "N/A" << setw(15) << irrigationFrequency 
             << setw(15) << calculateTotalYield() << endl;
    }

    void giveAdvice() const override {
        cout << "[Advisory for " << name << "]: Keep uniform moisture controls to minimize blight issues." << endl;
    }

    int getIrrigationFrequency() const { return irrigationFrequency; }
};

void saveCropData(const vector<unique_ptr<Crop>>& farmCrops, const string& filename) {
    ofstream outFile(filename);
    if (!outFile) return;

    for (const auto& crop : farmCrops) {
        CerealCrop* cereal = dynamic_cast<CerealCrop*>(crop.get());
        if (cereal) {
            outFile << "Cereal," << cereal->getName() << "," << cereal->getArea() << ","
                    << cereal->getSeason() << "," << cereal->getYieldPerAcre() << ","
                    << cereal->getFertilizerType() << "\n";
        } else {
            VegetableCrop* veg = dynamic_cast<VegetableCrop*>(crop.get());
            if (veg) {
                outFile << "Vegetable," << veg->getName() << "," << veg->getArea() << ","
                        << veg->getSeason() << "," << veg->getYieldPerAcre() << ","
                        << veg->getIrrigationFrequency() << "\n";
            }
        }
    }
    outFile.close();
}

void loadCropData(vector<unique_ptr<Crop>>& farmCrops, const string& filename) {
    ifstream inFile(filename);
    if (!inFile) return;

    string type, name, season, extra;
    string areaStr, yieldStr;

    while (getline(inFile, type, ',')) {
        getline(inFile, name, ',');
        getline(inFile, areaStr, ',');
        getline(inFile, season, ',');
        getline(inFile, yieldStr, ',');
        getline(inFile, extra);

        try {
            double area = stod(areaStr);
            double yield = stod(yieldStr);

            if (type == "Cereal") {
                farmCrops.push_back(make_unique<CerealCrop>(name, area, season, yield, extra));
            } else if (type == "Vegetable") {
                int freq = stoi(extra);
                farmCrops.push_back(make_unique<VegetableCrop>(name, area, season, yield, freq));
            }
        } catch (...) {}
    }
    inFile.close();
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    vector<unique_ptr<Crop>> farm;
    string filename = "farm_data.txt";
    loadCropData(farm, filename);

    int choice = 0;
    while (choice != 6) {
        cout << "\n=== Smart Farm Management System ===\n";
        cout << "1. Add Cereal Crop\n";
        cout << "2. Add Vegetable Crop\n";
        cout << "3. Display All Crops\n";
        cout << "4. View Farming Advice\n";
        cout << "5. Compare Two Crops by Yield\n";
        cout << "6. Save and Exit\n";
        cout << "Total tracked items in memory: " << Crop::getCropCount() << "\n";
        cout << "Enter choices (1-6): ";
        
        if (!(cin >> choice)) {
            clearInput();
            continue;
        }

        try {
            if (choice == 1 || choice == 2) {
                string name, season, fertilizer;
                double area, yield;
                int freq;

                cout << "Enter Name: ";
                cin >> name;
                cout << "Enter Area: ";
                if (!(cin >> area)) { clearInput(); throw invalid_argument("Invalid numeric type input."); }
                cout << "Enter Season: ";
                cin >> season;
                cout << "Enter Yield Per Acre: ";
                if (!(cin >> yield)) { clearInput(); throw invalid_argument("Invalid numeric type input."); }

                if (choice == 1) {
                    cout << "Enter Fertilizer Type: ";
                    cin >> fertilizer;
                    farm.push_back(make_unique<CerealCrop>(name, area, season, yield, fertilizer));
                } else {
                    cout << "Enter Irrigation Frequency (days): ";
                    if (!(cin >> freq)) { clearInput(); throw invalid_argument("Invalid numeric type input."); }
                    farm.push_back(make_unique<VegetableCrop>(name, area, season, yield, freq));
                }
                cout << "Crop added successfully.\n";
            } 
            else if (choice == 3) {
                if (farm.empty()) {
                    cout << "No crops registered yet.\n";
                    continue;
                }
                cout << "\n" << left << setw(15) << "Name" << setw(10) << "Area" << setw(12) << "Season" 
                     << setw(15) << "Yield/Acre" << setw(20) << "Fertilizer" << setw(15) << "Irrigation" 
                     << setw(15) << "Total Yield" << "\n";
                cout << string(105, '-') << "\n";
                for (const auto& crop : farm) {
                    crop->display();
                }
            } 
            else if (choice == 4) {
                if (farm.empty()) {
                    cout << "No items available to advise.\n";
                    continue;
                }
                cout << "\n=== Agricultural Advice Box ===\n";
                for (const auto& crop : farm) {
                    crop->giveAdvice();
                }
            } 
            else if (choice == 5) {
                if (farm.size() < 2) {
                    cout << "Need at least 2 entries in memory to compare.\n";
                    continue;
                }
                cout << "Available crops indexes:\n";
                for (size_t i = 0; i < farm.size(); ++i) {
                    cout << i << ": " << farm[i]->getName() << "\n";
                }
                size_t idx1, idx2;
                cout << "Enter index 1: ";
                cin >> idx1;
                cout << "Enter index 2: ";
                cin >> idx2;

                if (idx1 >= farm.size() || idx2 >= farm.size()) {
                    cout << "Index mismatch out of bounds.\n";
                    continue;
                }

                if (*farm[idx1] > *farm[idx2]) {
                    cout << farm[idx1]->getName() << " produces a higher yield output than " << farm[idx2]->getName() << ".\n";
                } else {
                    cout << farm[idx2]->getName() << " produces a higher or equal yield output than " << farm[idx1]->getName() << ".\n";
                }
            }
        } 
        catch (const exception& e) {
            cout << "Error occurred: " << e.what() << "\n";
        }
    }

    saveCropData(farm, filename);
    return 0;
}
