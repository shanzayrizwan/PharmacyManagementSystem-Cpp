#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <stdexcept>

using namespace std;

//==============================================================================
// TEMPLATE: Generic input validator
//==============================================================================

template <typename T>
bool isValid(T choice, T min, T max) {
    return (choice >= min && choice <= max);
}

//===================================================g===========================
// CUSTOM EXCEPTIONS (Exception Handling)
//==============================================================================

class PharmacyException : public exception {
protected:
    char message[200];
public:
    PharmacyException(const char* msg) {
        strcpy(message, msg);
    }
    virtual const char* what() const noexcept override {
        return message;
    }
};

class MedicineNotFoundException : public PharmacyException {
public:
    MedicineNotFoundException() : PharmacyException("[ERROR] Medicine not found in inventory!") {}
};

class OutOfStockException : public PharmacyException {
public:
    OutOfStockException(const char* name) : PharmacyException("") {
        strcpy(message, "[ERROR] Out of stock: ");
        strcat(message, name);
    }
};

class ExpiredMedicineException : public PharmacyException {
public:
    ExpiredMedicineException(const char* name) : PharmacyException("") {
        strcpy(message, "[ERROR] Medicine is expired: ");
        strcat(message, name);
    }
};

class InvalidPrescriptionException : public PharmacyException {
public:
    InvalidPrescriptionException() : PharmacyException("[ERROR] Valid prescription required for this medicine!") {}
};

class InvalidCredentialsException : public PharmacyException {
public:
    InvalidCredentialsException() : PharmacyException("[ERROR] Invalid username or password!") {}
};

//==============================================================================
// ABSTRACT BASE CLASS: Person (Abstraction)
//==============================================================================

class Person {
protected:
    char personName[50];
    int  age;
    char contact[20];
    char email[50];

public:
    Person() {
        strcpy(personName, "Unknown");
        strcpy(contact, "N/A");
        strcpy(email, "N/A");
        age = 0;
    }

    // Setters
    void setName(const char* n)    { strcpy(personName, n); }
    void setAge(int a)             { age = a; }
    void setContact(const char* c) { strcpy(contact, c); }
    void setEmail(const char* e)   { strcpy(email, e); }

    // Getters
    const char* getName()    const { return personName; }
    int         getAge()     const { return age; }
    const char* getContact() const { return contact; }
    const char* getEmail()   const { return email; }

    // Pure Virtual (Abstraction)
    virtual void displayInfo() const = 0;
    virtual void header()      const = 0;

    virtual ~Person() {}
};

//==============================================================================
// ABSTRACT BASE CLASS: Product (Abstraction)
//==============================================================================

class Product {
protected:
    int   id;
    char  name[50];
    float price;
    int   quantity;

public:
    Product() {
        id = 0;
        strcpy(name, "Unknown");
        price    = 0.0f;
        quantity = 0;
    }

    // Setters
    void setId(int i)           { id = i; }
    void setName(const char* n) { strcpy(name, n); }
    void setPrice(float p)      { price = p; }
    void setQuantity(int q)     { quantity = q; }

    // Getters
    int         getId()       const { return id; }
    const char* getName()     const { return name; }
    float       getPrice()    const { return price; }
    int         getQuantity() const { return quantity; }

    // Pure Virtuals (Abstraction)
    virtual void  displayDetails() const = 0;
    virtual float calculateTotal() const = 0;

    // Operator Overloading: << for displaying product
    friend ostream& operator<<(ostream& os, const Product& p);

    virtual ~Product() {}
};

ostream& operator<<(ostream& os, const Product& p) {
    os << "[" << p.id << "] " << p.name
       << " | Rs." << p.price
       << " | Qty: " << p.quantity;
    return os;
}

//==============================================================================
// DERIVED CLASS: Medicine extends Product (Inheritance)
//==============================================================================

class Medicine : public Product {
protected:
    char  genericName[50];
    char  category[30];
    char  manufacturer[50];
    char  batchNo[20];
    char  expiryDate[15];   // Format: MM/YYYY
    char  rackLocation[10];
    bool  expired;

public:
    Medicine() : Product() {
        strcpy(genericName,  "N/A");
        strcpy(category,     "General");
        strcpy(manufacturer, "N/A");
        strcpy(batchNo,      "N/A");
        strcpy(expiryDate,   "N/A");
        strcpy(rackLocation, "A1");
        expired = false;
    }

    void setData(int i, const char* n, const char* gn, const char* cat,
                 const char* mfg, const char* batch, float p, int q,
                 const char* exp, const char* rack, bool isExpired = false) {
        id = i;
        strcpy(name,         n);
        strcpy(genericName,  gn);
        strcpy(category,     cat);
        strcpy(manufacturer, mfg);
        strcpy(batchNo,      batch);
        price    = p;
        quantity = q;
        strcpy(expiryDate,   exp);
        strcpy(rackLocation, rack);
        expired  = isExpired;
    }

    // Getters
    const char* getCategory()     const { return category; }
    const char* getExpiryDate()   const { return expiryDate; }
    const char* getRackLocation() const { return rackLocation; }
    bool        isExpired()       const { return expired; }
    void        setExpired(bool e)      { expired = e; }

    // Overridden virtual functions (Polymorphism)
    virtual void displayDetails() const override {
        cout << "\n  ID      : " << id;
        cout << "\n  Name    : " << name << " (" << genericName << ")";
        cout << "\n  Category: " << category;
        cout << "\n  Maker   : " << manufacturer;
        cout << "\n  Batch   : " << batchNo;
        cout << "\n  Expiry  : " << expiryDate;
        cout << "\n  Rack    : " << rackLocation;
        cout << "\n  Price   : Rs." << price;
        cout << "\n  Stock   : " << quantity;
        if (expired) cout << "  [!! EXPIRED !!]";
    }

    virtual float calculateTotal() const override {
        return price * quantity;
    }

    // Row display for catalog table
    void displayRow(bool isAdmin) const {
        // ID
        cout << id;
        if (id < 10) cout << ".  ";
        else         cout << ". ";

        // Name
        cout << name;
        for (int i = 0; i < (18 - (int)strlen(name)); i++) cout << " ";

        // Price
        cout << "Rs." << price;
        int pLen = (price < 10) ? 10 : (price < 100) ? 9 : (price < 1000) ? 8 : 7;
        for (int i = 0; i < pLen; i++) cout << " ";

        // Category
        cout << category;
        for (int i = 0; i < (14 - (int)strlen(category)); i++) cout << " ";

        // Expiry
        cout << expiryDate;
        for (int i = 0; i < (10 - (int)strlen(expiryDate)); i++) cout << " ";

        // Stock
        if (expired) {
            cout << "[EXPIRED]";
        } else if (quantity <= 0) {
            cout << "[OUT OF STOCK]";
        } else {
            cout << quantity << " Left";
            if (isAdmin && quantity <= 5) cout << " [LOW STOCK!]";
        }
        cout << "\n";
    }
};

//==============================================================================
// DERIVED CLASS: PrescriptionMedicine extends Medicine (Multi-level Inheritance)
//==============================================================================

class PrescriptionMedicine : public Medicine {
private:
    bool requiresPrescription;
    char approvedDoctorType[50];

public:
    PrescriptionMedicine() : Medicine() {
        requiresPrescription = true;
        strcpy(approvedDoctorType, "General Physician");
    }

    void setPrescriptionData(bool req, const char* docType) {
        requiresPrescription = req;
        strcpy(approvedDoctorType, docType);
    }

    bool getRequiresPrescription() const { return requiresPrescription; }
    const char* getDoctorType()    const { return approvedDoctorType; }

    // Polymorphism: override displayDetails
    void displayDetails() const override {
        Medicine::displayDetails();
        cout << "\n  Type    : [PRESCRIPTION REQUIRED]";
        cout << "\n  Doctor  : " << approvedDoctorType;
    }

    float calculateTotal() const override {
        return price * quantity; // could add prescription fee
    }
};

//==============================================================================
// DERIVED CLASS: OTCMedicine extends Medicine (Hierarchical Inheritance)
//==============================================================================

class OTCMedicine : public Medicine {
private:
    char usageInstructions[100];

public:
    OTCMedicine() : Medicine() {
        strcpy(usageInstructions, "Follow label instructions.");
    }

    void setUsageInstructions(const char* inst) {
        strcpy(usageInstructions, inst);
    }

    const char* getUsageInstructions() const { return usageInstructions; }

    // Polymorphism: override displayDetails
    void displayDetails() const override {
        Medicine::displayDetails();
        cout << "\n  Type    : [OVER THE COUNTER - No Prescription Needed]";
        cout << "\n  Usage   : " << usageInstructions;
    }

    float calculateTotal() const override {
        return price * quantity;
    }
};

//==============================================================================
// CART ITEM: holds one line in the cart
//==============================================================================

struct CartItem {
    char  name[50];
    float price;
    int   qty;
    float lineTotal;
};

//==============================================================================
// CART CLASS: manages shopping session
//==============================================================================

class Cart {
private:
    CartItem items[50];
    int      count;
    float    discount;      // flat discount amount

public:
    Cart() { count = 0; discount = 0.0f; }

    void addItem(const char* n, float p, int q) {
        strcpy(items[count].name,  n);
        items[count].price     = p;
        items[count].qty       = q;
        items[count].lineTotal = p * q;
        count++;
    }

    // Operator Overloading: + to merge two carts
    Cart operator+(const Cart& other) const {
        Cart merged;
        for (int i = 0; i < count; i++)       merged.addItem(items[i].name, items[i].price, items[i].qty);
        for (int i = 0; i < other.count; i++) merged.addItem(other.items[i].name, other.items[i].price, other.items[i].qty);
        return merged;
    }

    void undoLast() {
        if (count > 0) {
            cout << "\n[UNDO] Removed last item: " << items[count-1].name;
            count--;
        } else {
            cout << "\n[!] Cart is already empty.";
        }
    }

    // Apply percentage discount
    void applyDiscount(float percent) {
        discount = getSubtotal() * (percent / 100.0f);
        cout << "\n[DISCOUNT] " << percent << "% applied. You save Rs." << discount;
    }

    // Overloaded: Apply flat discount
    void applyDiscount(float flat, bool isFlatDiscount) {
        if (isFlatDiscount) {
            discount = flat;
            cout << "\n[DISCOUNT] Flat Rs." << flat << " discount applied.";
        }
    }

    float getSubtotal() const {
        float total = 0;
        for (int i = 0; i < count; i++) total += items[i].lineTotal;
        return total;
    }

    float getGrandTotal() const {
        float gt = getSubtotal() - discount;
        return (gt < 0) ? 0 : gt;
    }

    int getCount() const { return count; }

    const CartItem& getItem(int i) const { return items[i]; }

    void clear() { count = 0; discount = 0.0f; }

    // Operator Overloading: << to display cart
    friend ostream& operator<<(ostream& os, const Cart& c);
};

ostream& operator<<(ostream& os, const Cart& c) {
    os << "\n\t---------------------------------------------------------";
    os << "\n\tITEM NAME            PRICE    QTY    TOTAL";
    os << "\n\t---------------------------------------------------------";
    for (int i = 0; i < c.count; i++) {
        os << "\n\t" << c.items[i].name;
        int pad = 21 - (int)strlen(c.items[i].name);
        for (int j = 0; j < pad; j++) os << " ";
        os << "Rs." << c.items[i].price;
        int ppad = (c.items[i].price < 100) ? 5 : (c.items[i].price < 1000) ? 4 : 3;
        for (int j = 0; j < ppad; j++) os << " ";
        os << c.items[i].qty << "      Rs." << c.items[i].lineTotal;
    }
    os << "\n\t---------------------------------------------------------";
    os << "\n\tSUBTOTAL:                              Rs." << c.getSubtotal();
    os << "\n\tGRAND TOTAL:                           Rs." << c.getGrandTotal();
    os << "\n\t---------------------------------------------------------";
    return os;
}

//==============================================================================
// PRESCRIPTION CLASS
//==============================================================================

class Prescription {
private:
    char  prescriptionID[20];
    char  doctorName[50];
    char  patientID[20];
    char  date[15];
    char  medicines[10][50];
    int   medCount;

public:
    Prescription() {
        strcpy(prescriptionID, "N/A");
        strcpy(doctorName,     "N/A");
        strcpy(patientID,      "N/A");
        strcpy(date,           "N/A");
        medCount = 0;
    }

    void setData(const char* pid, const char* doc, const char* patID, const char* d) {
        strcpy(prescriptionID, pid);
        strcpy(doctorName,     doc);
        strcpy(patientID,      patID);
        strcpy(date,           d);
    }

    void addMedicine(const char* medName) {
        if (medCount < 10) strcpy(medicines[medCount++], medName);
    }

    const char* getPrescriptionID() const { return prescriptionID; }
    const char* getDoctorName()     const { return doctorName; }
    const char* getPatientID()      const { return patientID; }

    bool containsMedicine(const char* medName) const {
        for (int i = 0; i < medCount; i++)
            if (strcmp(medicines[i], medName) == 0) return true;
        return false;
    }

    void display() const {
        cout << "\n  Prescription ID : " << prescriptionID;
        cout << "\n  Doctor          : " << doctorName;
        cout << "\n  Patient ID      : " << patientID;
        cout << "\n  Date            : " << date;
        cout << "\n  Medicines       : ";
        for (int i = 0; i < medCount; i++) cout << medicines[i] << (i < medCount-1 ? ", " : "");
    }
};

//==============================================================================
// PATIENT CLASS: Derived from Person (Inheritance)
//==============================================================================

class Patient : public Person {
private:
    char         patientID[20];
    Prescription prescriptions[5];
    int          prescCount;
    char         purchaseHistory[10][100];
    int          purchaseCount;

public:
    Patient() : Person() {
        strcpy(patientID, "P000");
        prescCount   = 0;
        purchaseCount = 0;
    }

    void setPatientID(const char* pid) { strcpy(patientID, pid); }
    const char* getPatientID() const   { return patientID; }

    void addPrescription(const Prescription& p) {
        if (prescCount < 5) prescriptions[prescCount++] = p;
    }

    void addPurchase(const char* summary) {
        if (purchaseCount < 10) strcpy(purchaseHistory[purchaseCount++], summary);
    }

    bool hasPrescriptionFor(const char* medName) const {
        for (int i = 0; i < prescCount; i++)
            if (prescriptions[i].containsMedicine(medName)) return true;
        return false;
    }

    // Polymorphism: implementing pure virtuals
    void displayInfo() const override {
        cout << "\n  Patient ID : " << patientID;
        cout << "\n  Name       : " << personName;
        cout << "\n  Age        : " << age;
        cout << "\n  Contact    : " << contact;
        cout << "\n  Email      : " << email;
    }

    void header() const override {
        cout << "\n==============================================================";
        cout << "\n  PATIENT: " << personName << "  |  ID: " << patientID;
        cout << "\n==============================================================\n";
    }

    void showHistory() const {
        cout << "\n--- Purchase History ---";
        if (purchaseCount == 0) { cout << "\n  No purchases yet."; return; }
        for (int i = 0; i < purchaseCount; i++)
            cout << "\n  " << (i+1) << ". " << purchaseHistory[i];
        cout << "\n--- Prescriptions ---";
        for (int i = 0; i < prescCount; i++) prescriptions[i].display();
    }
};

//==============================================================================
// PHARMACIST CLASS: Derived from Person (Hierarchical Inheritance)
//==============================================================================

class Pharmacist : public Person {
private:
    char  employeeID[20];
    char  shift[10];
    float salary;

public:
    Pharmacist() : Person() {
        strcpy(employeeID, "E000");
        strcpy(shift,      "Morning");
        salary = 0.0f;
    }

    void setEmployeeData(const char* eid, const char* sh, float sal) {
        strcpy(employeeID, eid);
        strcpy(shift, sh);
        salary = sal;
    }

    void displayInfo() const override {
        cout << "\n  Employee ID : " << employeeID;
        cout << "\n  Name        : " << personName;
        cout << "\n  Shift       : " << shift;
        cout << "\n  Salary      : Rs." << salary;
    }

    void header() const override {
        cout << "\n==============================================================";
        cout << "\n  PHARMACIST: " << personName << "  |  Shift: " << shift;
        cout << "\n==============================================================\n";
    }
};

//==============================================================================
// ADMIN CLASS: Derived from Person (Hierarchical Inheritance)
//==============================================================================

class Admin : public Person {
private:
    char username[30];
    char passwordHash[30];

    // Simple XOR hash for password
    void hashPassword(const char* pass, char* out) const {
        int len = strlen(pass);
        for (int i = 0; i < len; i++) out[i] = pass[i] ^ 0x5A;
        out[len] = '\0';
    }

public:
    Admin() : Person() {
        strcpy(username, "admin");
        // Default password: "1234"
        char tmp[30];
        hashPassword("1234", tmp);
        strcpy(passwordHash, tmp);
    }

    void setCredentials(const char* user, const char* pass) {
        strcpy(username, user);
        hashPassword(pass, passwordHash);
    }

    bool authenticate(const char* user, const char* pass) const {
        char hashed[30];
        hashPassword(pass, hashed);
        return (strcmp(username, user) == 0 && strcmp(passwordHash, hashed) == 0);
    }

    void displayInfo() const override {
        cout << "\n  Admin     : " << personName;
        cout << "\n  Username  : " << username;
    }

    void header() const override {
        cout << "\n==============================================================";
        cout << "\n  ADMIN PORTAL  |  User: " << username;
        cout << "\n==============================================================\n";
    }
};

//==============================================================================
// SUPPLIER CLASS
//==============================================================================

class Supplier {
private:
    char  supplierID[20];
    char  supplierName[50];
    char  contact[20];
    char  suppliedMeds[10][50];
    int   medCount;

public:
    Supplier() {
        strcpy(supplierID,   "S000");
        strcpy(supplierName, "Unknown");
        strcpy(contact,      "N/A");
        medCount = 0;
    }

    void setData(const char* sid, const char* sname, const char* con) {
        strcpy(supplierID,   sid);
        strcpy(supplierName, sname);
        strcpy(contact,      con);
    }

    void addSuppliedMedicine(const char* med) {
        if (medCount < 10) strcpy(suppliedMeds[medCount++], med);
    }

    void display() const {
        cout << "\n  Supplier ID : " << supplierID;
        cout << "\n  Name        : " << supplierName;
        cout << "\n  Contact     : " << contact;
        cout << "\n  Medicines   : ";
        for (int i = 0; i < medCount; i++) cout << suppliedMeds[i] << (i < medCount-1 ? ", " : "");
    }
};

//==============================================================================
// SEARCH ENGINE TEMPLATE: Generic search across arrays
//==============================================================================

template <typename T>
class SearchEngine {
public:
    // Search by name — returns index or -1
    int searchByName(T* arr, int size, const char* query) {
        for (int i = 0; i < size; i++) {
            if (strcmp(arr[i].getName(), query) == 0) return i;
        }
        return -1;
    }
};

//==============================================================================
// INVOICE CLASS
//==============================================================================

class Invoice {
private:
    char  invoiceID[20];
    char  patientID[20];
    Cart  cart;
    char  date[15];

public:
    Invoice() {
        strcpy(invoiceID, "INV000");
        strcpy(patientID, "P000");
        strcpy(date,      "N/A");
    }

    void setData(const char* iid, const char* pid, const Cart& c, const char* d) {
        strcpy(invoiceID, iid);
        strcpy(patientID, pid);
        cart = c;
        strcpy(date, d);
    }

    // Operator Overloading: << to print invoice
    friend ostream& operator<<(ostream& os, const Invoice& inv);
};

ostream& operator<<(ostream& os, const Invoice& inv) {
    os << "\n\t========================================================";
    os << "\n\t              PHARMACY INVOICE                          ";
    os << "\n\t========================================================";
    os << "\n\tInvoice ID : " << inv.invoiceID;
    os << "\n\tPatient ID : " << inv.patientID;
    os << "\n\tDate       : " << inv.date;
    os << inv.cart;
    os << "\n\t========================================================";
    os << "\n\t      THANK YOU FOR CHOOSING OUR PHARMACY!              ";
    os << "\n\t========================================================\n";
    return os;
}

//==============================================================================
// INVENTORY CLASS: manages all medicines + file I/O
//==============================================================================

class Inventory {
private:
    Medicine inventory[20];
    int      count;

    void saveToFile() {
        ofstream f("medicine_data.bin", ios::binary | ios::out);
        if (!f) { cout << "\n[ERROR] Cannot open file for saving!"; return; }
        f.write(reinterpret_cast<char*>(&count), sizeof(int));
        f.write(reinterpret_cast<char*>(inventory), sizeof(Medicine) * count);
        f.close();
    }

    void loadFromFile() {
        ifstream f("medicine_data.bin", ios::binary | ios::in);
        if (!f) {
            cout << "\n[SYSTEM] No saved data. Using defaults.";
            saveToFile();
            return;
        }
        f.read(reinterpret_cast<char*>(&count), sizeof(int));
        f.read(reinterpret_cast<char*>(inventory), sizeof(Medicine) * count);
        f.close();
        cout << "\n[SYSTEM] Inventory loaded (" << count << " medicines).";
    }

public:
    Inventory() {
        count = 12;
        // Default medicines
        inventory[0].setData(1,  "Panadol",    "Paracetamol",    "Tablet",    "GSK",      "B001", 20.5f,   100, "12/2026", "A1");
        inventory[1].setData(2,  "Arinac",     "Ibuprofen+PSE",  "Tablet",    "Abbott",   "B002", 15.0f,   100, "06/2026", "A2");
        inventory[2].setData(3,  "Brufen",     "Ibuprofen",      "Tablet",    "Abbott",   "B003", 10.0f,   100, "08/2026", "A3");
        inventory[3].setData(4,  "Disprin",    "Aspirin",        "Tablet",    "Bayer",    "B004",  5.0f,   100, "01/2025", "A4", true);
        inventory[4].setData(5,  "Hydryllin",  "Diphenhydramine","Syrup",     "Searle",   "B005", 120.0f,   50, "09/2026", "B1");
        inventory[5].setData(6,  "Cofee",      "Codeine",        "Syrup",     "Ferozsons","B006", 150.0f,   50, "03/2026", "B2");
        inventory[6].setData(7,  "Advent",     "Amoxicillin",    "Syrup",     "GSK",      "B007", 250.0f,   50, "11/2026", "B3");
        inventory[7].setData(8,  "Gravinate",  "Dimenhydrinate", "Syrup",     "Barrett",  "B008",  90.0f,   50, "07/2026", "B4");
        inventory[8].setData(9,  "Insulin",    "Insulin Glarg",  "Injection", "Novo",     "B009", 850.0f,   30, "05/2026", "C1");
        inventory[9].setData(10, "Dicloran",   "Diclofenac",     "Injection", "Searle",   "B010", 120.0f,   30, "04/2026", "C2");
        inventory[10].setData(11,"Venofer",    "Iron Sucrose",   "Injection", "Vifor",    "B011",1500.0f,   20, "10/2026", "C3");
        inventory[11].setData(12,"Avil",       "Pheniramine",    "Injection", "Sanofi",   "B012",  60.0f,   40, "02/2026", "C4");

        loadFromFile();
    }

    int getCount() const { return count; }

    Medicine& getMedicine(int i) { return inventory[i]; }

    void showCatalog(bool isAdmin) const {
        cout << "\n----------------------------------------------------------------------" << endl;
        cout << "NO.  NAME              PRICE       CATEGORY      EXPIRY    STOCK" << endl;
        cout << "----------------------------------------------------------------------" << endl;
        for (int i = 0; i < count; i++) inventory[i].displayRow(isAdmin);
        cout << "----------------------------------------------------------------------" << endl;
    }

    // Friend function: generates low-stock + expiry report (accesses private members)
    friend void generateAlertReport(const Inventory& inv);

    void restock(int idx, int amount) {
        if (!isValid(idx, 0, count-1)) throw MedicineNotFoundException();
        inventory[idx].setQuantity(inventory[idx].getQuantity() + amount);
        saveToFile();
        cout << "\n[SUCCESS] Restocked. New qty: " << inventory[idx].getQuantity();
    }

    Medicine* findByName(const char* n) {
        for (int i = 0; i < count; i++)
            if (strcmp(inventory[i].getName(), n) == 0) return &inventory[i];
        return nullptr;
    }

    void purchaseMedicine(int idx, int qty) {
        if (!isValid(idx, 0, count-1))    throw MedicineNotFoundException();
        if (inventory[idx].isExpired())    throw ExpiredMedicineException(inventory[idx].getName());
        if (inventory[idx].getQuantity() <= 0) throw OutOfStockException(inventory[idx].getName());
        if (qty > inventory[idx].getQuantity()) throw OutOfStockException(inventory[idx].getName());
        inventory[idx].setQuantity(inventory[idx].getQuantity() - qty);
        saveToFile();
    }

    void saveSalesLog(const char* patientName, const Cart& cart) {
        ofstream log("sales_report.txt", ios::app);
        if (!log) return;
        log << "\n--- Transaction | Patient: " << patientName << " ---\n";
        for (int i = 0; i < cart.getCount(); i++) {
            const CartItem& item = cart.getItem(i);
            log << "  " << item.name << " | Qty: " << item.qty << " | Rs." << item.lineTotal << "\n";
        }
        log << "  GRAND TOTAL: Rs." << cart.getGrandTotal() << "\n";
        log << "----------------------------------------------\n";
        log.close();
    }
};

// Friend function implementation
void generateAlertReport(const Inventory& inv) {
    cout << "\n========== LOW STOCK ALERT ==========";
    bool found = false;
    for (int i = 0; i < inv.count; i++) {
        if (inv.inventory[i].getQuantity() <= 5 && !inv.inventory[i].isExpired()) {
            cout << "\n  [LOW] " << inv.inventory[i].getName()
                 << " | Qty: " << inv.inventory[i].getQuantity()
                 << " | Rack: " << inv.inventory[i].getRackLocation();
            found = true;
        }
    }
    if (!found) cout << "\n  All medicines have sufficient stock.";

    cout << "\n\n========== EXPIRED MEDICINES ==========";
    found = false;
    for (int i = 0; i < inv.count; i++) {
        if (inv.inventory[i].isExpired()) {
            cout << "\n  [EXP] " << inv.inventory[i].getName()
                 << " | Expiry: " << inv.inventory[i].getExpiryDate()
                 << " | Rack: " << inv.inventory[i].getRackLocation();
            found = true;
        }
    }
    if (!found) cout << "\n  No expired medicines found.";
    cout << "\n";
}

//==============================================================================
// PHARMACY SYSTEM: Main controller
//==============================================================================

class PharmacySystem {
private:
    Inventory  inv;
    Admin      admin;
    Patient    patients[20];
    int        patientCount;
    Supplier   suppliers[10];
    int        supplierCount;

    SearchEngine<Medicine> medSearch;

public:
    PharmacySystem() {
        patientCount  = 0;
        supplierCount = 0;
        admin.setName("System Administrator");

        // Sample supplier
        suppliers[0].setData("S001", "MedCo Pharma", "0300-1234567");
        suppliers[0].addSuppliedMedicine("Panadol");
        suppliers[0].addSuppliedMedicine("Brufen");
        supplierCount = 1;
    }

    //--- ADMIN MODULE ---
    void adminPortal() {
        char user[30], pass[30];
        system("cls");
        admin.header();
        cout << "USERNAME: "; cin >> user;
        cout << "PASSWORD: "; cin >> pass;

        try {
            if (!admin.authenticate(user, pass)) throw InvalidCredentialsException();

            int ch;
            do {
                system("cls");
                admin.header();
                cout << "1. View Inventory\n";
                cout << "2. Restock Medicine\n";
                cout << "3. View Alerts & Reports\n";
                cout << "4. View Suppliers\n";
                cout << "5. View All Patients\n";
                cout << "0. Back\n";
                cout << "Choice: "; cin >> ch;

                switch (ch) {
                    case 1:
                        system("cls");
                        inv.showCatalog(true);
                        system("pause");
                        break;
                    case 2: {
                        system("cls");
                        inv.showCatalog(true);
                        int id, amt;
                        cout << "\nEnter Medicine Number to restock (1-" << inv.getCount() << "): "; cin >> id;
                        cout << "Enter quantity to add: "; cin >> amt;
                        try { inv.restock(id-1, amt); }
                        catch (MedicineNotFoundException& e) { cout << e.what(); }
                        system("pause");
                        break;
                    }
                    case 3:
                        system("cls");
                        generateAlertReport(inv);
                        system("pause");
                        break;
                    case 4:
                        system("cls");
                        cout << "\n===== SUPPLIERS =====";
                        for (int i = 0; i < supplierCount; i++) suppliers[i].display();
                        system("pause");
                        break;
                    case 5:
                        system("cls");
                        cout << "\n===== REGISTERED PATIENTS =====";
                        if (patientCount == 0) { cout << "\n  No patients registered."; }
                        for (int i = 0; i < patientCount; i++) {
                            patients[i].displayInfo();
                            patients[i].showHistory();
                            cout << "\n";
                        }
                        system("pause");
                        break;
                }
            } while (ch != 0);

        } catch (InvalidCredentialsException& e) {
            cout << "\n" << e.what();
            system("pause");
        }
    }

    //--- PATIENT REGISTRATION ---
    Patient& registerOrFindPatient() {
        char name[50], pid[20];
        cout << "\nEnter Patient Name: "; cin >> name;
        cout << "Enter Patient ID  : "; cin >> pid;

        // Find existing
        for (int i = 0; i < patientCount; i++)
            if (strcmp(patients[i].getPatientID(), pid) == 0) {
                cout << "\n[INFO] Existing patient found.";
                return patients[i];
            }

        // Register new
        patients[patientCount].setName(name);
        patients[patientCount].setPatientID(pid);
        cout << "\n[SUCCESS] Patient registered.";
        return patients[patientCount++];
    }

    //--- SALES / BILLING MODULE ---
    void startShopping() {
        system("cls");
        cout << "\n========== SALES COUNTER ==========\n";

        Patient& patient = registerOrFindPatient();
        Cart cart;
        char loopChoice;

        do {
            system("cls");
            patient.header();
            inv.showCatalog(false);

            int choice, qty;
            cout << "\nSelect Medicine Number (1-" << inv.getCount() << "): "; cin >> choice;

            try {
                if (!isValid(choice, 1, inv.getCount()))
                    throw MedicineNotFoundException();

                int idx = choice - 1;
                Medicine& med = inv.getMedicine(idx);

                // Check if it's prescription medicine
                // (In a full system PrescriptionMedicine objects would be in inventory)
                // Here we simulate: Insulin requires prescription
                if (strcmp(med.getName(), "Insulin") == 0) {
                    if (!patient.hasPrescriptionFor("Insulin"))
                        throw InvalidPrescriptionException();
                }

                cout << "Available: " << med.getQuantity() << "\nQty: "; cin >> qty;

                inv.purchaseMedicine(idx, qty);
                cart.addItem(med.getName(), med.getPrice(), qty);
                cout << "\n[ADDED] " << med.getName() << " x" << qty;

            } catch (MedicineNotFoundException& e)     { cout << "\n" << e.what(); }
              catch (OutOfStockException& e)            { cout << "\n" << e.what(); }
              catch (ExpiredMedicineException& e)       { cout << "\n" << e.what(); }
              catch (InvalidPrescriptionException& e)   { cout << "\n" << e.what(); }

            system("pause");
            cout << "\nAdd more? (y/n): "; cin >> loopChoice;

        } while (loopChoice == 'y' || loopChoice == 'Y');

        // Discount
        char dChoice;
        cout << "\nApply discount? (y/n): "; cin >> dChoice;
        if (dChoice == 'y' || dChoice == 'Y') {
            cout << "1. Percentage  2. Flat amount\nChoice: ";
            int dtype; cin >> dtype;
            float dval;
            if (dtype == 1) {
                cout << "Enter %: "; cin >> dval;
                cart.applyDiscount(dval);
            } else {
                cout << "Enter flat Rs: "; cin >> dval;
                cart.applyDiscount(dval, true);
            }
        }

        // Generate Invoice
        Invoice inv_obj;
        inv_obj.setData("INV001", patient.getPatientID(), cart, "2025-06-01");

        system("cls");
        cout << inv_obj;

        // Save to file
        inv.saveSalesLog(patient.getName(), cart);

        // Add to patient history
        char summary[100];
        strcpy(summary, "Purchased Rs.");
        // Append total manually
        int total = (int)cart.getGrandTotal();
        char tstr[20];
        sprintf(tstr, "%d", total);
        strcat(summary, tstr);
        patient.addPurchase(summary);
    }

    //--- PRESCRIPTION MODULE ---
    void prescriptionModule() {
        system("cls");
        cout << "\n===== PRESCRIPTION MANAGEMENT =====\n";

        char pid[20];
        cout << "Enter Patient ID: "; cin >> pid;

        Patient* p = nullptr;
        for (int i = 0; i < patientCount; i++)
            if (strcmp(patients[i].getPatientID(), pid) == 0) { p = &patients[i]; break; }

        if (!p) { cout << "\n[!] Patient not found. Register first."; system("pause"); return; }

        Prescription pres;
        char prid[20], doc[50], date[15], med[50];
        cout << "Prescription ID : "; cin >> prid;
        cout << "Doctor Name     : "; cin >> doc;
        cout << "Date (DD/MM/YY) : "; cin >> date;
        pres.setData(prid, doc, pid, date);

        int mcount;
        cout << "How many medicines in prescription? "; cin >> mcount;
        for (int i = 0; i < mcount; i++) {
            cout << "Medicine " << (i+1) << " name: "; cin >> med;
            pres.addMedicine(med);
        }

        p->addPrescription(pres);
        cout << "\n[SUCCESS] Prescription recorded.";
        system("pause");
    }
};

//==============================================================================
// MAIN FUNCTION
//==============================================================================

int main() {
    PharmacySystem pharmacy;
    int choice;

    while (true) {
        system("cls");
        cout << "==============================================\n";
        cout << "       PHARMACY MANAGEMENT SYSTEM            \n";
        cout << "==============================================\n";
        cout << "1. Admin Portal\n";
        cout << "2. Sales Counter (Customer)\n";
        cout << "3. Prescription Management\n";
        cout << "0. Exit\n";
        cout << "==============================================\n";
        cout << "Choice: "; cin >> choice;

        switch (choice) {
            case 1: pharmacy.adminPortal();        break;
            case 2: pharmacy.startShopping(); system("pause"); break;
            case 3: pharmacy.prescriptionModule(); break;
            case 0:
                cout << "\nShutting down...\n";
                return 0;
            default:
                cout << "\n[!] Invalid option.";
                system("pause");
        }
    }

    return 0;
}