#include "raylib.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cctype>

// ============================================================
// COLOR PALETTE  (light pinkish / rose theme)
// ============================================================
#define COL_BG          (Color){255, 240, 245, 255}   // lavender blush
#define COL_PANEL       (Color){255, 228, 235, 255}   // light pink
#define COL_PANEL2      (Color){255, 245, 248, 255}   // very light pink
#define COL_ACCENT      (Color){219, 112, 147, 255}   // pale violet red
#define COL_ACCENT2     (Color){255, 182, 193, 255}   // light pink highlight
#define COL_HEADER      (Color){199,  80, 120, 255}   // deep rose
#define COL_TEXT        (Color){ 80,  30,  50, 255}   // dark rose text
#define COL_SUBTEXT     (Color){160,  90, 120, 255}   // muted rose
#define COL_WHITE       WHITE
#define COL_GREEN       (Color){ 46, 160,  90, 255}
#define COL_RED         (Color){210,  70,  70, 255}
#define COL_ORANGE      (Color){230, 140,  60, 255}
#define COL_ROW_A       (Color){255, 235, 240, 255}
#define COL_ROW_B       (Color){255, 248, 251, 255}
#define COL_BORDER      (Color){230, 180, 200, 255}
#define COL_BTN_HOVER   (Color){240, 150, 175, 255}
#define COL_INPUT_BG    (Color){255, 252, 254, 255}

// ============================================================
// MEDICINE DATA
// ============================================================
struct Medicine {
    int   id;
    char  name[50];
    char  generic[50];
    char  category[30];
    char  expiry[15];
    float price;
    int   qty;
    bool  expired;
    char  rack[10];
};

static Medicine meds[20];
static int      medCount = 12;

void initMedicines() {
    auto set = [](int i, int id, const char* n, const char* g, const char* cat,
                  const char* exp, float p, int q, bool ex, const char* rack) {
        meds[i].id = id;
        strcpy(meds[i].name,     n);
        strcpy(meds[i].generic,  g);
        strcpy(meds[i].category, cat);
        strcpy(meds[i].expiry,   exp);
        meds[i].price   = p;
        meds[i].qty     = q;
        meds[i].expired = ex;
        strcpy(meds[i].rack, rack);
    };
    set(0,  1, "Panadol",   "Paracetamol",     "Tablet",    "12/2026", 20.5f, 100, false, "A1");
    set(1,  2, "Arinac",    "Ibuprofen+PSE",   "Tablet",    "06/2026", 15.0f, 100, false, "A2");
    set(2,  3, "Brufen",    "Ibuprofen",        "Tablet",    "08/2026", 10.0f, 100, false, "A3");
    set(3,  4, "Disprin",   "Aspirin",          "Tablet",    "01/2025",  5.0f, 100, true,  "A4");
    set(4,  5, "Hydryllin", "Diphenhydramine",  "Syrup",     "09/2026",120.0f,  50, false, "B1");
    set(5,  6, "Cofee",     "Codeine",          "Syrup",     "03/2026",150.0f,  50, false, "B2");
    set(6,  7, "Advent",    "Amoxicillin",      "Syrup",     "11/2026",250.0f,  50, false, "B3");
    set(7,  8, "Gravinate", "Dimenhydrinate",   "Syrup",     "07/2026", 90.0f,  50, false, "B4");
    set(8,  9, "Insulin",   "Insulin Glarg",    "Injection", "05/2026",850.0f,   4, false, "C1"); // low stock
    set(9, 10, "Dicloran",  "Diclofenac",       "Injection", "04/2026",120.0f,  30, false, "C2");
    set(10,11, "Venofer",   "Iron Sucrose",     "Injection", "10/2026",1500.0f, 20, false, "C3");
    set(11,12, "Avil",      "Pheniramine",      "Injection", "02/2026", 60.0f,   0, false, "C4"); // out of stock
}

// ============================================================
// CART
// ============================================================
struct CartItem { char name[50]; float price; int qty; };
static CartItem cart[50];
static int      cartCount = 0;

float cartSubtotal() {
    float s = 0; for (int i = 0; i < cartCount; i++) s += cart[i].price * cart[i].qty; return s;
}

// ============================================================
// UI HELPERS
// ============================================================
static Font fontMain;
static bool fontsLoaded = false;

// Draw rounded rect with border
void DrawRoundedBox(Rectangle r, float round, Color fill, Color border) {
    DrawRectangleRounded(r, round, 8, fill);
    DrawRectangleRoundedLines(r, round, 8, 2.0f, border);
}

// Fancy header banner
void DrawHeader(const char* title, const char* sub = "") {
    DrawRectangleGradientH(0, 0, GetScreenWidth(), 64, COL_HEADER, (Color){230, 100, 140, 255});
    // Decorative circles
    DrawCircle(GetScreenWidth()-40, 32, 48, (Color){255,255,255,18});
    DrawCircle(GetScreenWidth()-90, 10, 30, (Color){255,255,255,12});
    DrawCircle(50, 50, 60, (Color){255,255,255,10});
    DrawText("✦", 18, 18, 28, (Color){255,255,255,120});
    DrawText(title, 56, 14, 26, WHITE);
    if (strlen(sub) > 0) DrawText(sub, 56, 44, 13, (Color){255,230,238,255});
}

// Pink button
bool DrawButton(Rectangle r, const char* label, Color col = COL_ACCENT) {
    bool hov = CheckCollisionPointRec(GetMousePosition(), r);
    Color c = hov ? COL_BTN_HOVER : col;
    DrawRectangleRounded(r, 0.35f, 8, c);
    DrawRectangleRoundedLines(r, 0.35f, 8, 1.2f, COL_HEADER);
    int tw = MeasureText(label, 14);
    DrawText(label, r.x + (r.width - tw)/2, r.y + (r.height - 14)/2, 14, WHITE);
    return hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Outlined button (ghost)
bool DrawGhostButton(Rectangle r, const char* label) {
    bool hov = CheckCollisionPointRec(GetMousePosition(), r);
    DrawRectangleRounded(r, 0.35f, 8, hov ? COL_ACCENT2 : BLANK);
    DrawRectangleRoundedLines(r, 0.35f, 8, 1.5f, COL_ACCENT);
    int tw = MeasureText(label, 14);
    DrawText(label, r.x + (r.width - tw)/2, r.y + (r.height - 14)/2, 14, COL_ACCENT);
    return hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// Text input
struct TextInput {
    char buf[64];
    int  len;
    bool active;
    bool isPass;
    TextInput() { buf[0]='\0'; len=0; active=false; isPass=false; }
    void clear() { buf[0]='\0'; len=0; }
    void handle(Rectangle r) {
        active = CheckCollisionPointRec(GetMousePosition(), r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        if (!active) return;
        int k = GetCharPressed();
        while (k > 0) {
            if (k >= 32 && len < 62) { buf[len++] = (char)k; buf[len] = '\0'; }
            k = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && len > 0) buf[--len] = '\0';
    }
    void draw(Rectangle r, const char* placeholder) {
        bool hov = CheckCollisionPointRec(GetMousePosition(), r);
        Color bc = active ? COL_ACCENT : (hov ? COL_ACCENT2 : COL_BORDER);
        DrawRoundedBox(r, 0.3f, COL_INPUT_BG, bc);
        const char* show = (len == 0) ? placeholder : buf;
        Color tc = (len == 0) ? COL_SUBTEXT : COL_TEXT;
        if (isPass && len > 0) {
            char stars[64]; for (int i=0;i<len;i++) stars[i]='*'; stars[len]='\0';
            DrawText(stars, r.x+10, r.y+(r.height-16)/2, 16, tc);
        } else {
            DrawText(show, r.x+10, r.y+(r.height-16)/2, 16, tc);
        }
        if (active) {
            int cx = r.x+10+MeasureText((len==0?"":buf), 16);
            if ((int)(GetTime()*2)%2==0) DrawRectangle(cx, r.y+8, 2, r.height-16, COL_ACCENT);
        }
    }
};

// Number input (integer)
struct NumInput {
    char buf[16];
    int  len;
    bool active;
    NumInput() { buf[0]='\0'; len=0; active=false; }
    void clear() { buf[0]='\0'; len=0; }
    int  val() { return len>0 ? atoi(buf) : 0; }
    void handle(Rectangle r) {
        active = CheckCollisionPointRec(GetMousePosition(), r) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        if (!active) return;
        int k = GetCharPressed();
        while (k > 0) {
            if (k >= '0' && k <= '9' && len < 6) { buf[len++]=(char)k; buf[len]='\0'; }
            k = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && len > 0) buf[--len]='\0';
    }
    void draw(Rectangle r, const char* placeholder) {
        bool hov = CheckCollisionPointRec(GetMousePosition(), r);
        Color bc = active ? COL_ACCENT : (hov ? COL_ACCENT2 : COL_BORDER);
        DrawRoundedBox(r, 0.3f, COL_INPUT_BG, bc);
        const char* show = (len==0) ? placeholder : buf;
        Color tc = (len==0) ? COL_SUBTEXT : COL_TEXT;
        DrawText(show, r.x+10, r.y+(r.height-16)/2, 16, tc);
        if (active) {
            int cx = r.x+10+MeasureText((len==0?"":buf),16);
            if ((int)(GetTime()*2)%2==0) DrawRectangle(cx, r.y+8, 2, r.height-16, COL_ACCENT);
        }
    }
};

// Toast notification
struct Toast {
    char msg[120];
    float timer;
    bool isError;
    void show(const char* m, bool err=false) { strcpy(msg,m); timer=2.8f; isError=err; }
    void draw(int sw) {
        if (timer <= 0) return;
        timer -= GetFrameTime();
        float alpha = (timer < 0.5f) ? timer/0.5f : 1.0f;
        int tw = MeasureText(msg, 15);
        int bw = tw + 40, bh = 44;
        int bx = (sw - bw)/2, by = GetScreenHeight() - 80;
        Color col = isError ? (Color){220,80,80,(unsigned char)(220*alpha)}
                            : (Color){46,160,90,(unsigned char)(220*alpha)};
        DrawRectangleRounded({(float)bx,(float)by,(float)bw,(float)bh}, 0.4f, 8, col);
        DrawText(msg, bx+20, by+(bh-15)/2, 15, (Color){255,255,255,(unsigned char)(255*alpha)});
    }
};

// ============================================================
// SCREENS
// ============================================================
enum Screen {
    SCR_MAIN, SCR_ADMIN_LOGIN, SCR_ADMIN_DASH,
    SCR_INVENTORY, SCR_RESTOCK, SCR_ALERTS,
    SCR_SALES, SCR_CART, SCR_INVOICE,
    SCR_PRESCRIPTIONS, SCR_PATIENTS, SCR_SUPPLIERS,
    SCR_SEARCH, SCR_SETTINGS, SCR_ABOUT
};

static Screen curScreen = SCR_MAIN;
static Toast  toast;

// Scroll offsets
static float invScroll   = 0;
static float patScroll   = 0;
static float presScroll  = 0;
static int   selectedMed = -1;

// Admin login
static TextInput adminUser, adminPass;
static bool      loginError = false;

// Restock
static NumInput restockQty;

// Sales
static TextInput patName;
static NumInput  saleQty;
static int       saleSelected = -1;
static float     discountPct  = 0;

// Invoice
static float invoiceTotal = 0;

// ---- PATIENT RECORDS ----------------------------------------
struct PatientRecord {
    char id[20];
    char name[50];
    char contact[20];
    int  age;
    char lastVisit[20];
    char diagnosis[80];
};
static PatientRecord patients[20];
static int           patientCount = 0;

static TextInput patFormID, patFormName, patFormContact, patFormDiag;
static NumInput  patFormAge;
static bool      showPatForm = false;

// ---- PRESCRIPTION RECORDS -----------------------------------
struct PrescRecord {
    char presID[20];
    char patientID[20];
    char doctorName[50];
    char date[15];
    char medicines[5][50];
    int  medCount;
};
static PrescRecord prescriptions[30];
static int         prescCount = 0;

static TextInput presFormID, presFormPatID, presFormDoctor, presFormDate;
static TextInput presFormMed1, presFormMed2, presFormMed3;
static bool      showPresForm = false;

// ---- SUPPLIER DATA ------------------------------------------
struct SupplierRec {
    char id[20];
    char name[50];
    char contact[20];
    char email[50];
    char city[30];
    char medicines[4][50];
    int  medCount;
};
static SupplierRec suppliers[10];
static int         supplierCount = 0;

static TextInput supFormID, supFormName, supFormContact, supFormCity;
static bool      showSupForm = false;

// ---- SEARCH -------------------------------------------------
static TextInput searchQuery;
static int       searchResults[20];
static int       searchResultCount = 0;
static bool      searchDone = false;

// ---- SETTINGS -----------------------------------------------
static bool     settingLowStockAlert = true;
static bool     settingExpiryAlert   = true;
static bool     settingAutoSave      = true;
static int      settingLowThreshold  = 5;
static NumInput settingThresholdInput;
static TextInput settingNewPass, settingConfirmPass;
static bool      settingPassError = false;

// ---- SEED DEFAULT DATA --------------------------------------
void initDefaultData() {
    // Patients
    auto sp = [](int i, const char* id, const char* nm, const char* con,
                 int age, const char* visit, const char* diag) {
        strcpy(patients[i].id,        id);
        strcpy(patients[i].name,      nm);
        strcpy(patients[i].contact,   con);
        patients[i].age = age;
        strcpy(patients[i].lastVisit, visit);
        strcpy(patients[i].diagnosis, diag);
    };
    sp(0,"P001","Ayesha Khan",   "0300-1111111", 32, "22/04/2026", "Hypertension");
    sp(1,"P002","Ahmed Raza",    "0312-2222222", 45, "20/04/2026", "Diabetes Type II");
    sp(2,"P003","Fatima Malik",  "0333-3333333", 27, "18/04/2026", "Fever & Flu");
    sp(3,"P004","Usman Tariq",   "0321-4444444", 58, "15/04/2026", "Arthritis");
    patientCount = 4;

    // Prescriptions
    auto spr = [](int i, const char* pid, const char* pat, const char* doc,
                  const char* dt, const char* m1, const char* m2, const char* m3) {
        strcpy(prescriptions[i].presID,     pid);
        strcpy(prescriptions[i].patientID,  pat);
        strcpy(prescriptions[i].doctorName, doc);
        strcpy(prescriptions[i].date,       dt);
        prescriptions[i].medCount = 0;
        if (strlen(m1)>0) { strcpy(prescriptions[i].medicines[prescriptions[i].medCount++], m1); }
        if (strlen(m2)>0) { strcpy(prescriptions[i].medicines[prescriptions[i].medCount++], m2); }
        if (strlen(m3)>0) { strcpy(prescriptions[i].medicines[prescriptions[i].medCount++], m3); }
    };
    spr(0,"RX001","P001","Dr. Nadia Shah",   "22/04/2026","Panadol","Arinac","");
    spr(1,"RX002","P002","Dr. Kamran Ali",   "20/04/2026","Insulin","Brufen","");
    spr(2,"RX003","P003","Dr. Sara Baig",    "18/04/2026","Hydryllin","Cofee","Panadol");
    spr(3,"RX004","P004","Dr. Asif Mehmood", "15/04/2026","Dicloran","Brufen","");
    prescCount = 4;

    // Suppliers
    auto ss = [](int i, const char* id, const char* nm, const char* con,
                 const char* em, const char* city, const char* m1, const char* m2) {
        strcpy(suppliers[i].id,      id);
        strcpy(suppliers[i].name,    nm);
        strcpy(suppliers[i].contact, con);
        strcpy(suppliers[i].email,   em);
        strcpy(suppliers[i].city,    city);
        suppliers[i].medCount = 0;
        if (strlen(m1)>0) strcpy(suppliers[i].medicines[suppliers[i].medCount++], m1);
        if (strlen(m2)>0) strcpy(suppliers[i].medicines[suppliers[i].medCount++], m2);
    };
    ss(0,"S001","MedCo Pharma",    "0300-1234567","medco@pk.com",   "Karachi","Panadol","Brufen");
    ss(1,"S002","Abbott Pakistan", "0321-7654321","abbott@pk.com",  "Lahore", "Arinac","Brufen");
    ss(2,"S003","Searle Pakistan", "0333-9876543","searle@pk.com",  "Karachi","Hydryllin","Dicloran");
    supplierCount = 3;
}

// ============================================================
// SCREENS DRAW FUNCTIONS
// ============================================================

void DrawMainMenu() {
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(), COL_BG);
    DrawHeader("PHARMACY MANAGEMENT SYSTEM", "Rose Pharmacy — Karachi");

    // Decorative background blobs
    DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2-60, 320, (Color){255,210,225,30});

    int cx = GetScreenWidth()/2;
    float t = GetTime();

    // Title card
    DrawRoundedBox({(float)(cx-200), 80, 400, 60}, 0.3f, COL_PANEL, COL_BORDER);
    DrawText("Welcome", cx - MeasureText("Welcome",22)/2, 95, 22, COL_HEADER);
    DrawText("Select a module to continue", cx - MeasureText("Select a module to continue",13)/2, 122, 13, COL_SUBTEXT);

    // Menu buttons with icons
    struct { const char* icon; const char* label; Screen s; const char* desc; } items[] = {
        {"[A]", "Admin Portal",     SCR_ADMIN_LOGIN, "Manage inventory & reports"},
        {"[S]", "Sales Counter",    SCR_SALES,       "Process customer purchases"},
    };

    for (int i = 0; i < 2; i++) {
        float py = 175 + i * 130 + sinf(t + i) * 3;
        Rectangle card = {(float)(cx - 200), py, 400.0f, 100.0f};
        bool hov = CheckCollisionPointRec(GetMousePosition(), card);
        Color bg = hov ? COL_PANEL : COL_PANEL2;
        DrawRoundedBox(card, 0.15f, bg, COL_BORDER);
        if (hov) DrawRectangleRoundedLines(card, 0.15f, 8, 2.5f, COL_ACCENT);

        // Icon circle
        DrawCircle(cx-160, py+50, 24, COL_ACCENT2);
        DrawText(items[i].icon, cx-175, py+39, 16, COL_HEADER);
        DrawText(items[i].label, cx-125, py+28, 20, COL_TEXT);
        DrawText(items[i].desc,  cx-125, py+56, 13, COL_SUBTEXT);
        DrawText("->", cx+155, py+42, 18, COL_ACCENT);

        if (hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) curScreen = items[i].s;
    }

    DrawText("v1.0 | Raylib GUI Edition", cx - MeasureText("v1.0 | Raylib GUI Edition",12)/2,
             GetScreenHeight()-28, 12, COL_SUBTEXT);
}

void DrawAdminLogin() {
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(), COL_BG);
    DrawHeader("ADMIN PORTAL", "Secure login required");

    int cx = GetScreenWidth()/2;

    // Card
    DrawRoundedBox({(float)(cx-180), 90, 360, 280}, 0.12f, COL_PANEL, COL_BORDER);

    DrawText("Administrator Login", cx - MeasureText("Administrator Login",18)/2, 110, 18, COL_HEADER);
    DrawText("Default: admin / 1234", cx - MeasureText("Default: admin / 1234",12)/2, 136, 12, COL_SUBTEXT);

    DrawText("Username", cx-160, 170, 14, COL_TEXT);
    adminUser.draw({(float)(cx-160), 188, 320, 36}, "Enter username");
    adminUser.handle({(float)(cx-160), 188, 320, 36});

    DrawText("Password", cx-160, 238, 14, COL_TEXT);
    adminPass.isPass = true;
    adminPass.draw({(float)(cx-160), 256, 320, 36}, "Enter password");
    adminPass.handle({(float)(cx-160), 256, 320, 36});

    if (loginError) DrawText("[!] Invalid credentials", cx - MeasureText("[!] Invalid credentials",13)/2, 304, 13, COL_RED);

    if (DrawButton({(float)(cx-160), 320, 155, 38}, "LOGIN")) {
        bool ok = (strcmp(adminUser.buf,"admin")==0 && strcmp(adminPass.buf,"1234")==0);
        if (ok) {
            loginError = false; adminUser.clear(); adminPass.clear();
            curScreen = SCR_ADMIN_DASH;
            toast.show("Welcome, Administrator!");
        } else { loginError = true; }
    }
    if (DrawGhostButton({(float)(cx+5), 320, 155, 38}, "BACK")) {
        loginError=false; adminUser.clear(); adminPass.clear(); curScreen = SCR_MAIN;
    }
}

void DrawAdminDash() {
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(), COL_BG);
    DrawHeader("ADMIN DASHBOARD", "Rose Pharmacy — Administrator");

    int cx = GetScreenWidth()/2;

    struct { const char* title; const char* icon; const char* sub; Screen s; } cards[] = {
        {"Inventory",       "[I]", "View all medicines",      SCR_INVENTORY},
        {"Restock",         "[R]", "Update stock levels",     SCR_RESTOCK},
        {"Alerts & Reports","[!]", "Low stock / expired",     SCR_ALERTS},
    };

    for (int i = 0; i < 3; i++) {
        float t = GetTime();
        Rectangle r = {(float)(cx-240 + i*170), 90, 155, 130};
        bool hov = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRoundedBox(r, 0.15f, hov ? COL_PANEL : COL_PANEL2, COL_BORDER);
        if (hov) DrawRectangleRoundedLines(r, 0.15f, 8, 2.5f, COL_ACCENT);

        DrawCircle(r.x+30, r.y+35, 20, COL_ACCENT2);
        DrawText(cards[i].icon, r.x+16, r.y+25, 16, COL_HEADER);
        DrawText(cards[i].title, r.x+10, r.y+62, 15, COL_TEXT);
        DrawText(cards[i].sub,   r.x+10, r.y+83, 11, COL_SUBTEXT);
        if (hov) DrawText("Open ->", r.x+10, r.y+106, 12, COL_ACCENT);

        if (hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) curScreen = cards[i].s;
    }

    // Quick stats
    int totalQty = 0, expired = 0, lowStock = 0;
    for (int i = 0; i < medCount; i++) {
        totalQty += meds[i].qty;
        if (meds[i].expired) expired++;
        if (meds[i].qty <= 5 && !meds[i].expired) lowStock++;
    }

    float sy = 250;
    DrawText("Quick Stats", cx - MeasureText("Quick Stats",18)/2, sy, 18, COL_HEADER);

    struct { const char* l; int v; Color c; } stats[] = {
        {"Total Medicines", medCount,  COL_ACCENT},
        {"Total Stock",     totalQty,  COL_GREEN},
        {"Expired",         expired,   COL_RED},
        {"Low Stock",       lowStock,  COL_ORANGE},
    };
    for (int i = 0; i < 4; i++) {
        Rectangle sr = {(float)(cx-280+i*145), sy+30, 135, 80};
        DrawRoundedBox(sr, 0.2f, COL_PANEL, COL_BORDER);
        char vstr[16]; sprintf(vstr, "%d", stats[i].v);
        DrawText(vstr, sr.x + (sr.width - MeasureText(vstr,28))/2, sr.y+10, 28, stats[i].c);
        DrawText(stats[i].l, sr.x + (sr.width - MeasureText(stats[i].l,11))/2, sr.y+50, 11, COL_SUBTEXT);
    }

    if (DrawButton({(float)(cx-60), (float)(GetScreenHeight()-68), 120, 38}, "LOGOUT")) {
        curScreen = SCR_MAIN;
    }
}

void DrawInventory(bool isAdmin) {
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(), COL_BG);
    DrawHeader("MEDICINE INVENTORY", isAdmin ? "Admin View" : "Customer View");

    int sw = GetScreenWidth();
    // Column headers
    float hy = 72;
    DrawRoundedBox({20, hy, (float)(sw-40), 30}, 0.2f, COL_ACCENT, COL_BORDER);
    DrawText("ID", 30, hy+7, 13, WHITE);
    DrawText("NAME",          90,  hy+7, 13, WHITE);
    DrawText("GENERIC",       235, hy+7, 13, WHITE);
    DrawText("CATEGORY",      400, hy+7, 13, WHITE);
    DrawText("PRICE",         520, hy+7, 13, WHITE);
    DrawText("EXPIRY",        620, hy+7, 13, WHITE);
    DrawText("STOCK",         720, hy+7, 13, WHITE);

    // Table rows with scroll
    float rowH = 36, startY = 108;
    int visible = (int)((GetScreenHeight() - startY - 70) / rowH);

    // Scroll
    float wheel = GetMouseWheelMove();
    invScroll -= wheel * 30;
    if (invScroll < 0) invScroll = 0;
    float maxScroll = fmaxf(0, medCount * rowH - visible * rowH);
    if (invScroll > maxScroll) invScroll = maxScroll;

    BeginScissorMode(0, (int)startY, sw, GetScreenHeight() - (int)startY - 60);
    for (int i = 0; i < medCount; i++) {
        float ry = startY + i * rowH - invScroll;
        if (ry < startY - rowH || ry > GetScreenHeight()) continue;

        Color rowBg = (i%2==0) ? COL_ROW_A : COL_ROW_B;
        if (selectedMed == i) rowBg = COL_ACCENT2;
        bool hov = CheckCollisionPointRec(GetMousePosition(), {20, ry, (float)(sw-40), rowH-2});
        if (hov) rowBg = (Color){255,200,220,255};

        DrawRectangleRounded({20, ry, (float)(sw-40), rowH-2}, 0.15f, 6, rowBg);

        char idStr[8]; sprintf(idStr, "%d", meds[i].id);
        DrawText(idStr, 32, ry+9, 14, COL_TEXT);
        DrawText(meds[i].name,     90,  ry+9, 14, COL_TEXT);
        DrawText(meds[i].generic,  235, ry+9, 13, COL_SUBTEXT);
        DrawText(meds[i].category, 400, ry+9, 13, COL_TEXT);

        char prStr[16]; sprintf(prStr, "Rs.%.0f", meds[i].price);
        DrawText(prStr, 520, ry+9, 13, COL_TEXT);
        DrawText(meds[i].expiry, 620, ry+9, 13, meds[i].expired ? COL_RED : COL_TEXT);

        // Stock badge
        if (meds[i].expired) {
            DrawRectangleRounded({720,ry+4,80,22}, 0.4f, 8, COL_RED);
            DrawText("EXPIRED", 725, ry+8, 11, WHITE);
        } else if (meds[i].qty <= 0) {
            DrawRectangleRounded({720,ry+4,80,22}, 0.4f, 8, COL_RED);
            DrawText("OUT", 743, ry+8, 11, WHITE);
        } else {
            Color sc = (meds[i].qty <= 5) ? COL_ORANGE : COL_GREEN;
            DrawRectangleRounded({720,ry+4,60,22}, 0.4f, 8, sc);
            char qstr[16]; sprintf(qstr, "%d", meds[i].qty);
            DrawText(qstr, 720+(60-MeasureText(qstr,11))/2, ry+8, 11, WHITE);
            if (isAdmin && meds[i].qty <= 5) DrawText("LOW!", 785, ry+8, 11, COL_ORANGE);
        }

        if (hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) selectedMed = i;
    }
    EndScissorMode();

    // Back / action buttons
    if (isAdmin) {
        if (DrawButton({20, (float)(GetScreenHeight()-52), 110, 36}, "BACK")) curScreen = SCR_ADMIN_DASH;
    } else {
        if (DrawButton({20, (float)(GetScreenHeight()-52), 110, 36}, "BACK")) curScreen = SCR_SALES;
    }
}

void DrawRestock() {
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(), COL_BG);
    DrawHeader("RESTOCK MEDICINE", "Update inventory quantities");

    DrawInventory(true); // draw table in background — we'll overlay a panel

    // Side panel overlay
    int sw = GetScreenWidth();
    DrawRoundedBox({(float)(sw-300), 68, 280, 300}, 0.08f, COL_PANEL, COL_ACCENT);
    DrawText("Restock Panel", sw-290, 82, 16, COL_HEADER);

    if (selectedMed >= 0) {
        DrawText(meds[selectedMed].name, sw-290, 108, 15, COL_TEXT);
        char info[60]; sprintf(info, "Current stock: %d", meds[selectedMed].qty);
        DrawText(info, sw-290, 130, 13, COL_SUBTEXT);
        DrawText("Add quantity:", sw-290, 160, 13, COL_TEXT);
        restockQty.draw({(float)(sw-290), 178, 160, 36}, "Enter qty");
        restockQty.handle({(float)(sw-290), 178, 160, 36});

        if (DrawButton({(float)(sw-290), 228, 160, 36}, "RESTOCK")) {
            int add = restockQty.val();
            if (add > 0) {
                meds[selectedMed].qty += add;
                char msg[80]; sprintf(msg, "Restocked %s (+%d units)", meds[selectedMed].name, add);
                toast.show(msg); restockQty.clear();
            } else toast.show("Enter a valid quantity!", true);
        }
    } else {
        DrawText("Select a medicine", sw-290, 120, 13, COL_SUBTEXT);
        DrawText("from the table first.", sw-290, 140, 13, COL_SUBTEXT);
    }

    if (DrawButton({(float)(sw-290), 320, 160, 36}, "BACK")) { curScreen = SCR_ADMIN_DASH; selectedMed=-1; }
}

void DrawAlerts() {
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(), COL_BG);
    DrawHeader("ALERTS & REPORTS", "Low stock and expired medicines");

    int sw = GetScreenWidth();
    float y = 80;

    // Low stock
    DrawText("LOW STOCK ALERTS", 30, y, 16, COL_HEADER);
    y += 24;
    bool anyLow = false;
    for (int i = 0; i < medCount; i++) {
        if (meds[i].qty <= 5 && !meds[i].expired) {
            anyLow = true;
            DrawRoundedBox({30, y, (float)(sw-60), 34}, 0.2f, (Color){255,240,220,255}, COL_ORANGE);
            char info[100]; sprintf(info, "[!] %-15s | Stock: %d | Rack: %s | Price: Rs.%.0f",
                                   meds[i].name, meds[i].qty, meds[i].rack, meds[i].price);
            DrawText(info, 44, y+9, 13, (Color){160,80,20,255});
            y += 40;
        }
    }
    if (!anyLow) { DrawText("All medicines have sufficient stock.", 30, y, 13, COL_GREEN); y += 28; }

    y += 14;
    DrawLine(30, (int)y, sw-30, (int)y, COL_BORDER); y += 14;
    DrawText("EXPIRED MEDICINES", 30, y, 16, COL_HEADER); y += 24;
    bool anyExp = false;
    for (int i = 0; i < medCount; i++) {
        if (meds[i].expired) {
            anyExp = true;
            DrawRoundedBox({30, y, (float)(sw-60), 34}, 0.2f, (Color){255,230,230,255}, COL_RED);
            char info[100]; sprintf(info, "[X] %-15s | Expiry: %s | Rack: %s",
                                   meds[i].name, meds[i].expiry, meds[i].rack);
            DrawText(info, 44, y+9, 13, (Color){160,30,30,255});
            y += 40;
        }
    }
    if (!anyExp) { DrawText("No expired medicines.", 30, y, 13, COL_GREEN); }

    if (DrawButton({30, (float)(GetScreenHeight()-52), 110, 36}, "BACK")) curScreen = SCR_ADMIN_DASH;
}

void DrawSalesScreen() {
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(), COL_BG);
    DrawHeader("SALES COUNTER", "Add medicines to cart");

    int sw = GetScreenWidth();

    // Patient input row
    DrawRoundedBox({20, 72, 500, 46}, 0.2f, COL_PANEL, COL_BORDER);
    DrawText("Patient:", 32, 84, 14, COL_TEXT);
    patName.draw({96, 78, 260, 34}, "Patient name");
    patName.handle({96, 78, 260, 34});
    DrawText("(press after entering)", 362, 86, 12, COL_SUBTEXT);

    // Inventory table (scrollable, left portion)
    float hy = 128;
    DrawRoundedBox({20, hy, (float)(sw-320), 30}, 0.2f, COL_ACCENT, COL_BORDER);
    DrawText("ID",   32,  hy+8, 13, WHITE);
    DrawText("NAME", 80,  hy+8, 13, WHITE);
    DrawText("GENERIC", 220, hy+8, 13, WHITE);
    DrawText("PRICE",   370, hy+8, 13, WHITE);
    DrawText("STOCK",   460, hy+8, 13, WHITE);

    float rowH = 34, startY = hy+32;
    int visible = (int)((GetScreenHeight() - startY - 70) / rowH);

    float wheel = GetMouseWheelMove();
    invScroll -= wheel * 30;
    if (invScroll < 0) invScroll = 0;
    float maxScroll = fmaxf(0, medCount * rowH - visible * rowH);
    if (invScroll > maxScroll) invScroll = maxScroll;

    BeginScissorMode(20, (int)startY, sw-320, GetScreenHeight() - (int)startY - 60);
    for (int i = 0; i < medCount; i++) {
        float ry = startY + i * rowH - invScroll;
        if (ry < startY - rowH || ry > GetScreenHeight()) continue;

        Color rowBg = (i%2==0) ? COL_ROW_A : COL_ROW_B;
        if (saleSelected == i) rowBg = COL_ACCENT2;
        bool hov = CheckCollisionPointRec(GetMousePosition(), {20, ry, (float)(sw-320), rowH-2});
        if (hov) rowBg = (Color){255,200,220,255};
        DrawRectangleRounded({20, ry, (float)(sw-320), rowH-2}, 0.15f, 6, rowBg);

        char idStr[8]; sprintf(idStr, "%d", meds[i].id);
        DrawText(idStr,          32, ry+8, 13, COL_TEXT);
        DrawText(meds[i].name,   80, ry+8, 14, COL_TEXT);
        DrawText(meds[i].generic,220, ry+8, 12, COL_SUBTEXT);
        char prStr[16]; sprintf(prStr, "Rs.%.0f", meds[i].price);
        DrawText(prStr, 370, ry+8, 13, COL_TEXT);

        if (meds[i].expired) {
            DrawRectangleRounded({460,ry+4,70,22},0.4f,6,COL_RED); DrawText("EXPIRED",464,ry+8,11,WHITE);
        } else if (meds[i].qty <= 0) {
            DrawRectangleRounded({460,ry+4,50,22},0.4f,6,COL_RED); DrawText("OUT",470,ry+8,11,WHITE);
        } else {
            char qs[10]; sprintf(qs, "%d", meds[i].qty);
            DrawRectangleRounded({460,ry+4,50,22},0.4f,6,COL_GREEN); DrawText(qs,460+(50-MeasureText(qs,11))/2,ry+8,11,WHITE);
        }

        if (hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) saleSelected = i;
    }
    EndScissorMode();

    // RIGHT PANEL: add to cart
    float px = sw - 295;
    DrawRoundedBox({px, 72, 280, GetScreenHeight() - 130.0f}, 0.08f, COL_PANEL, COL_ACCENT);
    DrawText("Add to Cart", px+10, 82, 16, COL_HEADER);

    if (saleSelected >= 0) {
        DrawText(meds[saleSelected].name, px+10, 106, 15, COL_TEXT);
        char inf[40]; sprintf(inf, "Rs.%.0f | Stock: %d", meds[saleSelected].price, meds[saleSelected].qty);
        DrawText(inf, px+10, 128, 12, COL_SUBTEXT);
        DrawText("Quantity:", px+10, 158, 13, COL_TEXT);
        saleQty.draw({px+10, 176, 120, 34}, "Qty");
        saleQty.handle({px+10, 176, 120, 34});

        if (DrawButton({px+10, 224, 140, 34}, "ADD TO CART")) {
            int q = saleQty.val();
            Medicine& m = meds[saleSelected];
            if (m.expired) toast.show("Medicine is expired!", true);
            else if (m.qty <= 0) toast.show("Out of stock!", true);
            else if (q <= 0 || q > m.qty) toast.show("Invalid quantity!", true);
            else {
                // Check duplicate
                bool found = false;
                for (int i=0; i<cartCount; i++) if (strcmp(cart[i].name, m.name)==0) { cart[i].qty+=q; found=true; break; }
                if (!found) { strcpy(cart[cartCount].name, m.name); cart[cartCount].price=m.price; cart[cartCount].qty=q; cartCount++; }
                m.qty -= q;
                char msg[60]; sprintf(msg, "Added %s x%d", m.name, q); toast.show(msg);
                saleQty.clear();
            }
        }
    } else {
        DrawText("Select a medicine", px+10, 110, 13, COL_SUBTEXT);
        DrawText("from the table.", px+10, 130, 13, COL_SUBTEXT);
    }

    // Cart summary
    DrawLine(px+10, (int)(px < 0 ? 0 : 280), sw-20, (int)(px < 0 ? 0 : 280), COL_BORDER);
    float cy = 290;
    DrawText("Cart Summary", px+10, cy, 14, COL_HEADER); cy += 22;
    int maxShow = 5;
    int start = cartCount > maxShow ? cartCount - maxShow : 0;
    for (int i = start; i < cartCount; i++) {
        char line[60]; sprintf(line, "%s x%d", cart[i].name, cart[i].qty);
        DrawText(line, px+10, cy, 12, COL_TEXT);
        char pr[20]; sprintf(pr, "Rs.%.0f", cart[i].price * cart[i].qty);
        DrawText(pr, sw-20-MeasureText(pr,12), cy, 12, COL_ACCENT);
        cy += 18;
    }
    if (cartCount > maxShow) DrawText("...", px+10, cy, 12, COL_SUBTEXT);

    DrawLine(px+10, GetScreenHeight()-125, sw-20, GetScreenHeight()-125, COL_BORDER);
    char tot[30]; sprintf(tot, "Total: Rs.%.0f", cartSubtotal());
    DrawText(tot, px+10, GetScreenHeight()-118, 14, COL_HEADER);

    if (DrawButton({px+10, (float)(GetScreenHeight()-92), 130, 34}, "CHECKOUT")) {
        if (cartCount == 0) toast.show("Cart is empty!", true);
        else { invoiceTotal = cartSubtotal(); curScreen = SCR_INVOICE; }
    }
    if (DrawGhostButton({px+145, (float)(GetScreenHeight()-92), 120, 34}, "CLEAR")) {
        // restore qty
        for (int i=0; i<cartCount; i++) for (int j=0;j<medCount;j++) if(strcmp(cart[i].name,meds[j].name)==0) meds[j].qty+=cart[i].qty;
        cartCount=0; toast.show("Cart cleared.");
    }

    if (DrawButton({20, (float)(GetScreenHeight()-52), 110, 36}, "MAIN MENU")) { curScreen = SCR_MAIN; }
}

void DrawInvoice() {
    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(), COL_BG);
    DrawHeader("INVOICE", "Thank you for your purchase!");

    int sw = GetScreenWidth();
    float iw = 500, ix = (sw - iw)/2, iy = 80;

    // Invoice card
    DrawRoundedBox({ix, iy, iw, (float)(GetScreenHeight()-150)}, 0.05f, COL_WHITE, COL_BORDER);

    // Header stripe
    DrawRectangleRounded({ix, iy, iw, 50}, 0.05f, 8, COL_ACCENT);
    DrawText("PHARMACY INVOICE", ix+20, iy+16, 18, WHITE);
    DrawText("Rose Pharmacy, Karachi", ix+iw-MeasureText("Rose Pharmacy, Karachi",12)-15, iy+20, 12, (Color){255,230,240,200});

    float y = iy + 60;
    char info[60]; sprintf(info, "Patient: %s", patName.len>0 ? patName.buf : "Walk-in Customer");
    DrawText(info, ix+20, y, 14, COL_TEXT); y += 20;
    DrawText("Date: 24/04/2026", ix+20, y, 13, COL_SUBTEXT); y += 24;

    // Line
    DrawLine(ix+10, (int)y, ix+iw-10, (int)y, COL_BORDER); y += 10;
    DrawText("ITEM", ix+20, y, 13, COL_HEADER);
    DrawText("QTY",  ix+280, y, 13, COL_HEADER);
    DrawText("PRICE", ix+330, y, 13, COL_HEADER);
    DrawText("TOTAL", ix+420, y, 13, COL_HEADER); y += 20;
    DrawLine(ix+10, (int)y, ix+iw-10, (int)y, COL_BORDER); y += 8;

    for (int i = 0; i < cartCount; i++) {
        Color rowBg = (i%2==0) ? COL_ROW_A : COL_ROW_B;
        DrawRectangleRounded({ix+5, y, iw-10, 22}, 0.2f, 6, rowBg);
        DrawText(cart[i].name, ix+20, y+4, 13, COL_TEXT);
        char qs[8]; sprintf(qs, "%d", cart[i].qty); DrawText(qs, ix+285, y+4, 13, COL_TEXT);
        char ps[16]; sprintf(ps, "Rs.%.0f", cart[i].price); DrawText(ps, ix+330, y+4, 13, COL_TEXT);
        char ts[16]; sprintf(ts, "Rs.%.0f", cart[i].price*cart[i].qty); DrawText(ts, ix+420, y+4, 13, COL_ACCENT);
        y += 26;
    }

    y += 6;
    DrawLine(ix+10, (int)y, ix+iw-10, (int)y, COL_ACCENT); y += 8;
    char tot[40]; sprintf(tot, "GRAND TOTAL:  Rs.%.2f", invoiceTotal);
    DrawText(tot, ix+iw-MeasureText(tot,16)-20, y, 16, COL_HEADER); y += 30;
    DrawText("Thank you for choosing Rose Pharmacy!", ix+(iw-MeasureText("Thank you for choosing Rose Pharmacy!",13))/2, y, 13, COL_SUBTEXT);

    if (DrawButton({(float)(sw/2-130), (float)(GetScreenHeight()-68), 120, 38}, "NEW SALE")) {
        cartCount = 0; patName.clear(); saleSelected = -1; curScreen = SCR_SALES;
        toast.show("New sale started.");
    }
    if (DrawButton({(float)(sw/2+10), (float)(GetScreenHeight()-68), 120, 38}, "MAIN MENU", COL_SUBTEXT)) {
        cartCount = 0; patName.clear(); saleSelected = -1; curScreen = SCR_MAIN;
    }
}

// ============================================================
// DRAW: PATIENTS SCREEN
// ============================================================
void DrawPatientsScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), COL_BG);
    DrawHeader("PATIENT RECORDS", "Registered patients and history");

    int sw = GetScreenWidth();
    float btnY = GetScreenHeight() - 54.0f;

    // Toggle add-patient form
    if (DrawButton({(float)(sw - 180), 78, 160, 34}, "+ ADD PATIENT")) showPatForm = !showPatForm;

    if (showPatForm) {
        // Form card overlay
        float fx = (sw - 480) / 2.0f, fy = 72;
        DrawRoundedBox({fx, fy, 480, 310}, 0.08f, COL_PANEL, COL_ACCENT);
        DrawText("New Patient", fx + 14, fy + 12, 17, COL_HEADER);

        DrawText("Patient ID",  fx + 14, fy + 44,  13, COL_TEXT);
        patFormID.draw({fx + 14, fy + 60, 210, 34}, "e.g. P005");
        patFormID.handle({fx + 14, fy + 60, 210, 34});

        DrawText("Full Name",   fx + 240, fy + 44, 13, COL_TEXT);
        patFormName.draw({fx + 240, fy + 60, 226, 34}, "Patient name");
        patFormName.handle({fx + 240, fy + 60, 226, 34});

        DrawText("Contact",     fx + 14, fy + 106, 13, COL_TEXT);
        patFormContact.draw({fx + 14, fy + 122, 210, 34}, "03XX-XXXXXXX");
        patFormContact.handle({fx + 14, fy + 122, 210, 34});

        DrawText("Age",         fx + 240, fy + 106, 13, COL_TEXT);
        patFormAge.draw({fx + 240, fy + 122, 100, 34}, "Age");
        patFormAge.handle({fx + 240, fy + 122, 100, 34});

        DrawText("Diagnosis",   fx + 14, fy + 168, 13, COL_TEXT);
        patFormDiag.draw({fx + 14, fy + 184, 452, 34}, "Primary diagnosis");
        patFormDiag.handle({fx + 14, fy + 184, 452, 34});

        if (DrawButton({fx + 14, fy + 234, 130, 36}, "SAVE")) {
            if (patFormID.len > 0 && patFormName.len > 0 && patientCount < 20) {
                int idx = patientCount++;
                strcpy(patients[idx].id,        patFormID.buf);
                strcpy(patients[idx].name,       patFormName.buf);
                strcpy(patients[idx].contact,    patFormContact.len > 0 ? patFormContact.buf : "N/A");
                patients[idx].age = patFormAge.val() > 0 ? patFormAge.val() : 0;
                strcpy(patients[idx].lastVisit,  "24/04/2026");
                strcpy(patients[idx].diagnosis,  patFormDiag.len > 0 ? patFormDiag.buf : "N/A");
                patFormID.clear(); patFormName.clear(); patFormContact.clear();
                patFormDiag.clear(); patFormAge.clear();
                showPatForm = false;
                toast.show("Patient registered successfully!");
            } else {
                toast.show("ID and Name are required.", true);
            }
        }
        if (DrawGhostButton({fx + 160, fy + 234, 100, 36}, "CANCEL")) {
            showPatForm = false;
            patFormID.clear(); patFormName.clear(); patFormContact.clear();
            patFormDiag.clear(); patFormAge.clear();
        }
        return; // don't draw table behind form
    }

    // Table header
    float hy = 120;
    DrawRoundedBox({20, hy, (float)(sw - 40), 30}, 0.2f, COL_ACCENT, COL_BORDER);
    DrawText("ID",        30,       hy + 8, 13, WHITE);
    DrawText("NAME",      110,      hy + 8, 13, WHITE);
    DrawText("AGE",       290,      hy + 8, 13, WHITE);
    DrawText("CONTACT",   340,      hy + 8, 13, WHITE);
    DrawText("LAST VISIT",490,      hy + 8, 13, WHITE);
    DrawText("DIAGNOSIS", 620,      hy + 8, 13, WHITE);

    float rowH = 36, startY = hy + 34;
    float wheel = GetMouseWheelMove();
    patScroll -= wheel * 30;
    if (patScroll < 0) patScroll = 0;
    float maxScroll = fmaxf(0, patientCount * rowH - (GetScreenHeight() - startY - 70));
    if (patScroll > maxScroll) patScroll = maxScroll;

    BeginScissorMode(0, (int)startY, sw, GetScreenHeight() - (int)startY - 60);
    for (int i = 0; i < patientCount; i++) {
        float ry = startY + i * rowH - patScroll;
        if (ry < startY - rowH || ry > GetScreenHeight()) continue;
        Color rowBg = (i % 2 == 0) ? COL_ROW_A : COL_ROW_B;
        bool hov = CheckCollisionPointRec(GetMousePosition(), {20, ry, (float)(sw - 40), rowH - 2});
        if (hov) rowBg = COL_ACCENT2;
        DrawRectangleRounded({20, ry, (float)(sw - 40), rowH - 2}, 0.15f, 6, rowBg);
        DrawText(patients[i].id,        30,  ry + 9, 13, COL_TEXT);
        DrawText(patients[i].name,      110, ry + 9, 14, COL_TEXT);
        char ageStr[8]; sprintf(ageStr, "%d", patients[i].age);
        DrawText(ageStr,                290, ry + 9, 13, COL_TEXT);
        DrawText(patients[i].contact,   340, ry + 9, 13, COL_SUBTEXT);
        DrawText(patients[i].lastVisit, 490, ry + 9, 13, COL_SUBTEXT);
        DrawText(patients[i].diagnosis, 620, ry + 9, 13, COL_TEXT);
    }
    EndScissorMode();

    if (patientCount == 0) DrawText("No patients registered yet.", 30, startY + 14, 13, COL_SUBTEXT);

    if (DrawButton({20, btnY, 110, 36}, "BACK")) curScreen = SCR_ADMIN_DASH;
}

// ============================================================
// DRAW: PRESCRIPTIONS SCREEN
// ============================================================
void DrawPrescriptionsScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), COL_BG);
    DrawHeader("PRESCRIPTIONS", "Doctor prescriptions and medicine orders");

    int sw = GetScreenWidth();
    float btnY = GetScreenHeight() - 54.0f;

    if (DrawButton({(float)(sw - 200), 78, 178, 34}, "+ ADD PRESCRIPTION")) showPresForm = !showPresForm;

    if (showPresForm) {
        float fx = (sw - 520) / 2.0f, fy = 68;
        DrawRoundedBox({fx, fy, 520, 330}, 0.08f, COL_PANEL, COL_ACCENT);
        DrawText("New Prescription", fx + 14, fy + 12, 17, COL_HEADER);

        DrawText("Prescription ID", fx + 14, fy + 44, 13, COL_TEXT);
        presFormID.draw({fx + 14, fy + 60, 170, 32}, "e.g. RX005");
        presFormID.handle({fx + 14, fy + 60, 170, 32});

        DrawText("Patient ID", fx + 200, fy + 44, 13, COL_TEXT);
        presFormPatID.draw({fx + 200, fy + 60, 140, 32}, "e.g. P001");
        presFormPatID.handle({fx + 200, fy + 60, 140, 32});

        DrawText("Date", fx + 356, fy + 44, 13, COL_TEXT);
        presFormDate.draw({fx + 356, fy + 60, 148, 32}, "DD/MM/YYYY");
        presFormDate.handle({fx + 356, fy + 60, 148, 32});

        DrawText("Doctor Name", fx + 14, fy + 104, 13, COL_TEXT);
        presFormDoctor.draw({fx + 14, fy + 120, 490, 32}, "Doctor full name");
        presFormDoctor.handle({fx + 14, fy + 120, 490, 32});

        DrawText("Medicine 1", fx + 14, fy + 164, 13, COL_TEXT);
        presFormMed1.draw({fx + 14, fy + 180, 155, 32}, "Medicine name");
        presFormMed1.handle({fx + 14, fy + 180, 155, 32});

        DrawText("Medicine 2", fx + 182, fy + 164, 13, COL_TEXT);
        presFormMed2.draw({fx + 182, fy + 180, 155, 32}, "Medicine name");
        presFormMed2.handle({fx + 182, fy + 180, 155, 32});

        DrawText("Medicine 3", fx + 350, fy + 164, 13, COL_TEXT);
        presFormMed3.draw({fx + 350, fy + 180, 154, 32}, "Medicine name");
        presFormMed3.handle({fx + 350, fy + 180, 154, 32});

        if (DrawButton({fx + 14, fy + 228, 130, 36}, "SAVE")) {
            if (presFormID.len > 0 && presFormPatID.len > 0 && prescCount < 30) {
                int idx = prescCount++;
                strcpy(prescriptions[idx].presID,     presFormID.buf);
                strcpy(prescriptions[idx].patientID,  presFormPatID.buf);
                strcpy(prescriptions[idx].doctorName, presFormDoctor.len > 0 ? presFormDoctor.buf : "Unknown");
                strcpy(prescriptions[idx].date,       presFormDate.len > 0 ? presFormDate.buf : "N/A");
                prescriptions[idx].medCount = 0;
                if (presFormMed1.len > 0) strcpy(prescriptions[idx].medicines[prescriptions[idx].medCount++], presFormMed1.buf);
                if (presFormMed2.len > 0) strcpy(prescriptions[idx].medicines[prescriptions[idx].medCount++], presFormMed2.buf);
                if (presFormMed3.len > 0) strcpy(prescriptions[idx].medicines[prescriptions[idx].medCount++], presFormMed3.buf);
                presFormID.clear(); presFormPatID.clear(); presFormDoctor.clear();
                presFormDate.clear(); presFormMed1.clear(); presFormMed2.clear(); presFormMed3.clear();
                showPresForm = false;
                toast.show("Prescription saved!");
            } else {
                toast.show("Prescription ID and Patient ID required.", true);
            }
        }
        if (DrawGhostButton({fx + 160, fy + 228, 100, 36}, "CANCEL")) {
            showPresForm = false;
            presFormID.clear(); presFormPatID.clear(); presFormDoctor.clear();
            presFormDate.clear(); presFormMed1.clear(); presFormMed2.clear(); presFormMed3.clear();
        }
        return;
    }

    // Table header
    float hy = 120;
    DrawRoundedBox({20, hy, (float)(sw - 40), 30}, 0.2f, COL_ACCENT, COL_BORDER);
    DrawText("RX ID",       30,  hy + 8, 13, WHITE);
    DrawText("PATIENT ID",  130, hy + 8, 13, WHITE);
    DrawText("DOCTOR",      240, hy + 8, 13, WHITE);
    DrawText("DATE",        420, hy + 8, 13, WHITE);
    DrawText("MEDICINES",   520, hy + 8, 13, WHITE);

    float rowH = 38, startY = hy + 34;
    float wheel = GetMouseWheelMove();
    presScroll -= wheel * 30;
    if (presScroll < 0) presScroll = 0;
    float maxScroll = fmaxf(0, prescCount * rowH - (GetScreenHeight() - startY - 70));
    if (presScroll > maxScroll) presScroll = maxScroll;

    BeginScissorMode(0, (int)startY, sw, GetScreenHeight() - (int)startY - 60);
    for (int i = 0; i < prescCount; i++) {
        float ry = startY + i * rowH - presScroll;
        if (ry < startY - rowH || ry > GetScreenHeight()) continue;
        Color rowBg = (i % 2 == 0) ? COL_ROW_A : COL_ROW_B;
        bool hov = CheckCollisionPointRec(GetMousePosition(), {20, ry, (float)(sw - 40), rowH - 2});
        if (hov) rowBg = COL_ACCENT2;
        DrawRectangleRounded({20, ry, (float)(sw - 40), rowH - 2}, 0.15f, 6, rowBg);

        DrawText(prescriptions[i].presID,     30,  ry + 11, 13, COL_HEADER);
        DrawText(prescriptions[i].patientID,  130, ry + 11, 13, COL_TEXT);
        DrawText(prescriptions[i].doctorName, 240, ry + 11, 13, COL_TEXT);
        DrawText(prescriptions[i].date,       420, ry + 11, 13, COL_SUBTEXT);

        // Medicines as comma list
        char medList[150] = "";
        for (int m = 0; m < prescriptions[i].medCount; m++) {
            strcat(medList, prescriptions[i].medicines[m]);
            if (m < prescriptions[i].medCount - 1) strcat(medList, ", ");
        }
        DrawText(medList, 520, ry + 11, 12, COL_TEXT);
    }
    EndScissorMode();

    if (prescCount == 0) DrawText("No prescriptions recorded yet.", 30, startY + 14, 13, COL_SUBTEXT);

    if (DrawButton({20, btnY, 110, 36}, "BACK")) curScreen = SCR_ADMIN_DASH;
}

// ============================================================
// DRAW: SUPPLIERS SCREEN
// ============================================================
void DrawSuppliersScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), COL_BG);
    DrawHeader("SUPPLIERS", "Medicine supplier directory");

    int sw = GetScreenWidth();
    float btnY = GetScreenHeight() - 54.0f;

    if (DrawButton({(float)(sw - 180), 78, 158, 34}, "+ ADD SUPPLIER")) showSupForm = !showSupForm;

    if (showSupForm) {
        float fx = (sw - 480) / 2.0f, fy = 72;
        DrawRoundedBox({fx, fy, 480, 260}, 0.08f, COL_PANEL, COL_ACCENT);
        DrawText("New Supplier", fx + 14, fy + 12, 17, COL_HEADER);

        DrawText("Supplier ID",  fx + 14, fy + 44, 13, COL_TEXT);
        supFormID.draw({fx + 14, fy + 60, 170, 32}, "e.g. S004");
        supFormID.handle({fx + 14, fy + 60, 170, 32});

        DrawText("Company Name", fx + 200, fy + 44, 13, COL_TEXT);
        supFormName.draw({fx + 200, fy + 60, 265, 32}, "Supplier company name");
        supFormName.handle({fx + 200, fy + 60, 265, 32});

        DrawText("Contact", fx + 14, fy + 104, 13, COL_TEXT);
        supFormContact.draw({fx + 14, fy + 120, 210, 32}, "Phone number");
        supFormContact.handle({fx + 14, fy + 120, 210, 32});

        DrawText("City", fx + 240, fy + 104, 13, COL_TEXT);
        supFormCity.draw({fx + 240, fy + 120, 225, 32}, "City");
        supFormCity.handle({fx + 240, fy + 120, 225, 32});

        if (DrawButton({fx + 14, fy + 174, 130, 36}, "SAVE")) {
            if (supFormID.len > 0 && supFormName.len > 0 && supplierCount < 10) {
                int idx = supplierCount++;
                strcpy(suppliers[idx].id,      supFormID.buf);
                strcpy(suppliers[idx].name,    supFormName.buf);
                strcpy(suppliers[idx].contact, supFormContact.len > 0 ? supFormContact.buf : "N/A");
                strcpy(suppliers[idx].city,    supFormCity.len > 0 ? supFormCity.buf : "N/A");
                strcpy(suppliers[idx].email,   "N/A");
                suppliers[idx].medCount = 0;
                supFormID.clear(); supFormName.clear(); supFormContact.clear(); supFormCity.clear();
                showSupForm = false;
                toast.show("Supplier added!");
            } else {
                toast.show("ID and Name required.", true);
            }
        }
        if (DrawGhostButton({fx + 160, fy + 174, 100, 36}, "CANCEL")) {
            showSupForm = false;
            supFormID.clear(); supFormName.clear(); supFormContact.clear(); supFormCity.clear();
        }
        return;
    }

    // Cards layout for suppliers
    float cx = 30, cy = 120;
    float cardW = (sw - 80) / 3.0f, cardH = 140;
    for (int i = 0; i < supplierCount; i++) {
        float sx = cx + (i % 3) * (cardW + 10);
        float sy = cy + (i / 3) * (cardH + 14) - invScroll * 0.5f;
        bool hov = CheckCollisionPointRec(GetMousePosition(), {sx, sy, cardW, cardH});
        DrawRoundedBox({sx, sy, cardW, cardH}, 0.1f, hov ? COL_PANEL : COL_PANEL2, COL_BORDER);
        if (hov) DrawRectangleRoundedLines({sx, sy, cardW, cardH}, 0.1f, 8, 2.0f, COL_ACCENT);

        // Supplier icon circle
        DrawCircle((int)(sx + 28), (int)(sy + 28), 18, COL_ACCENT2);
        DrawText("S", (int)(sx + 22), (int)(sy + 20), 16, COL_HEADER);

        DrawText(suppliers[i].name,    (int)(sx + 52), (int)(sy + 14), 15, COL_TEXT);
        char idStr[30]; sprintf(idStr, "ID: %s", suppliers[i].id);
        DrawText(idStr,                (int)(sx + 52), (int)(sy + 34), 12, COL_SUBTEXT);
        DrawText(suppliers[i].contact, (int)(sx + 14), (int)(sy + 62), 13, COL_TEXT);
        DrawText(suppliers[i].city,    (int)(sx + 14), (int)(sy + 82), 13, COL_SUBTEXT);

        // Medicine tags
        float tx = sx + 14, ty = sy + 106;
        for (int m = 0; m < suppliers[i].medCount && tx < sx + cardW - 20; m++) {
            int tw = MeasureText(suppliers[i].medicines[m], 11) + 10;
            DrawRectangleRounded({tx, ty, (float)tw, 20}, 0.4f, 6, COL_ACCENT2);
            DrawText(suppliers[i].medicines[m], (int)(tx + 5), (int)(ty + 4), 11, COL_HEADER);
            tx += tw + 6;
        }
    }

    if (supplierCount == 0) DrawText("No suppliers on record.", 30, 130, 13, COL_SUBTEXT);

    if (DrawButton({20, btnY, 110, 36}, "BACK")) curScreen = SCR_ADMIN_DASH;
}

// ============================================================
// DRAW: SEARCH SCREEN
// ============================================================
void DrawSearchScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), COL_BG);
    DrawHeader("MEDICINE SEARCH", "Find medicines by name or category");

    int sw = GetScreenWidth();
    float btnY = GetScreenHeight() - 54.0f;

    // Search bar
    DrawRoundedBox({20, 76, (float)(sw - 160), 38}, 0.3f, COL_INPUT_BG, COL_ACCENT);
    searchQuery.draw({20, 76, (float)(sw - 160), 38}, "Type medicine name or category...");
    searchQuery.handle({20, 76, (float)(sw - 160), 38});

    if (DrawButton({(float)(sw - 130), 76, 110, 38}, "SEARCH")) {
        searchResultCount = 0;
        searchDone = true;
        if (searchQuery.len > 0) {
            for (int i = 0; i < medCount; i++) {
                // Case-insensitive partial match on name or category
                char lq[64], ln[50], lc[30];
                strcpy(lq, searchQuery.buf);
                strcpy(ln, meds[i].name);
                strcpy(lc, meds[i].category);
                for (int k = 0; lq[k]; k++) lq[k] = (char)tolower(lq[k]);
                for (int k = 0; ln[k]; k++) ln[k] = (char)tolower(ln[k]);
                for (int k = 0; lc[k]; k++) lc[k] = (char)tolower(lc[k]);
                if (strstr(ln, lq) || strstr(lc, lq)) {
                    searchResults[searchResultCount++] = i;
                }
            }
        }
    }

    // Also search on Enter
    if (IsKeyPressed(KEY_ENTER) && searchQuery.active) {
        searchResultCount = 0;
        searchDone = true;
        for (int i = 0; i < medCount; i++) {
            char lq[64], ln[50];
            strcpy(lq, searchQuery.buf);
            strcpy(ln, meds[i].name);
            for (int k = 0; lq[k]; k++) lq[k] = (char)tolower(lq[k]);
            for (int k = 0; ln[k]; k++) ln[k] = (char)tolower(ln[k]);
            if (strstr(ln, lq)) searchResults[searchResultCount++] = i;
        }
    }

    float y = 130;
    if (!searchDone) {
        DrawText("Enter a search term above and click SEARCH.", 30, y, 13, COL_SUBTEXT);
        DrawText("You can search by medicine name or category (e.g. Tablet, Syrup, Injection).", 30, y + 24, 12, COL_SUBTEXT);
    } else if (searchResultCount == 0) {
        DrawText("No medicines found matching your search.", 30, y, 13, COL_RED);
    } else {
        char countMsg[60]; sprintf(countMsg, "%d result(s) found:", searchResultCount);
        DrawText(countMsg, 30, y, 14, COL_HEADER); y += 28;

        // Column header
        DrawRoundedBox({20, y, (float)(sw - 40), 28}, 0.2f, COL_ACCENT, COL_BORDER);
        DrawText("NAME",     32,  y + 7, 13, WHITE);
        DrawText("GENERIC",  190, y + 7, 13, WHITE);
        DrawText("CATEGORY", 360, y + 7, 13, WHITE);
        DrawText("PRICE",    500, y + 7, 13, WHITE);
        DrawText("EXPIRY",   600, y + 7, 13, WHITE);
        DrawText("STOCK",    710, y + 7, 13, WHITE);
        y += 32;

        for (int r = 0; r < searchResultCount; r++) {
            int i = searchResults[r];
            Color rowBg = (r % 2 == 0) ? COL_ROW_A : COL_ROW_B;
            DrawRectangleRounded({20, y, (float)(sw - 40), 32}, 0.15f, 6, rowBg);
            DrawText(meds[i].name,     32,  y + 8, 14, COL_TEXT);
            DrawText(meds[i].generic,  190, y + 8, 12, COL_SUBTEXT);
            DrawText(meds[i].category, 360, y + 8, 13, COL_TEXT);
            char pr[16]; sprintf(pr, "Rs.%.0f", meds[i].price);
            DrawText(pr, 500, y + 8, 13, COL_TEXT);
            DrawText(meds[i].expiry, 600, y + 8, 13, meds[i].expired ? COL_RED : COL_TEXT);
            if (meds[i].expired) {
                DrawRectangleRounded({710, y + 4, 70, 20}, 0.4f, 6, COL_RED);
                DrawText("EXPIRED", 714, y + 8, 11, WHITE);
            } else {
                Color sc = meds[i].qty <= 5 ? COL_ORANGE : COL_GREEN;
                char qs[10]; sprintf(qs, "%d", meds[i].qty);
                DrawRectangleRounded({710, y + 4, 50, 20}, 0.4f, 6, sc);
                DrawText(qs, 710 + (50 - MeasureText(qs, 11)) / 2, y + 8, 11, WHITE);
            }
            y += 36;
        }
    }

    if (DrawButton({20, btnY, 110, 36}, "BACK")) {
        curScreen = SCR_ADMIN_DASH;
        searchDone = false;
        searchQuery.clear();
        searchResultCount = 0;
    }
}

// ============================================================
// DRAW: SETTINGS SCREEN
// ============================================================

// Small toggle switch helper
bool DrawToggle(float x, float y, bool value, const char* label) {
    // Track
    Color trackCol = value ? COL_ACCENT : COL_BORDER;
    DrawRectangleRounded({x, y + 2, 40, 20}, 0.5f, 8, trackCol);
    // Knob
    float kx = value ? x + 22 : x + 2;
    DrawCircle((int)(kx + 8), (int)(y + 12), 8, WHITE);
    DrawText(label, (int)(x + 50), (int)(y + 2), 14, COL_TEXT);
    Rectangle toggleRect = {x, y, 40, 24};
    if (CheckCollisionPointRec(GetMousePosition(), toggleRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        return !value;
    return value;
}

void DrawSettingsScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), COL_BG);
    DrawHeader("SETTINGS", "System configuration and preferences");

    int sw = GetScreenWidth();
    float btnY = GetScreenHeight() - 54.0f;
    float px = 40, py = 84;
    float cardW = (sw - 80.0f) / 2.0f;

    // ---- Left column: Alerts & Thresholds ----
    DrawRoundedBox({px, py, cardW - 10, 240}, 0.08f, COL_PANEL, COL_BORDER);
    DrawText("Alert Settings", (int)(px + 14), (int)(py + 12), 16, COL_HEADER);

    settingLowStockAlert = DrawToggle(px + 14, py + 48,  settingLowStockAlert, "Low Stock Alerts");
    settingExpiryAlert   = DrawToggle(px + 14, py + 86,  settingExpiryAlert,   "Expiry Alerts");
    settingAutoSave      = DrawToggle(px + 14, py + 124, settingAutoSave,      "Auto-save Data");

    DrawText("Low Stock Threshold (qty):", (int)(px + 14), (int)(py + 162), 13, COL_TEXT);
    settingThresholdInput.draw({px + 14, py + 180, 100, 32}, "e.g. 5");
    settingThresholdInput.handle({px + 14, py + 180, 100, 32});

    if (DrawButton({px + 124, py + 180, 110, 32}, "APPLY")) {
        int v = settingThresholdInput.val();
        if (v > 0 && v <= 50) {
            settingLowThreshold = v;
            settingThresholdInput.clear();
            toast.show("Threshold updated!");
        } else {
            toast.show("Enter a value between 1 and 50.", true);
        }
    }
    char thrStr[40]; sprintf(thrStr, "Current threshold: %d", settingLowThreshold);
    DrawText(thrStr, (int)(px + 14), (int)(py + 220), 12, COL_SUBTEXT);

    // ---- Right column: Change Password ----
    float rx = px + cardW + 10;
    DrawRoundedBox({rx, py, cardW - 10, 240}, 0.08f, COL_PANEL, COL_BORDER);
    DrawText("Change Admin Password", (int)(rx + 14), (int)(py + 12), 16, COL_HEADER);
    DrawText("New Password:", (int)(rx + 14), (int)(py + 52), 13, COL_TEXT);
    settingNewPass.isPass = true;
    settingNewPass.draw({rx + 14, py + 70, cardW - 38, 32}, "New password");
    settingNewPass.handle({rx + 14, py + 70, cardW - 38, 32});

    DrawText("Confirm Password:", (int)(rx + 14), (int)(py + 114), 13, COL_TEXT);
    settingConfirmPass.isPass = true;
    settingConfirmPass.draw({rx + 14, py + 132, cardW - 38, 32}, "Confirm password");
    settingConfirmPass.handle({rx + 14, py + 132, cardW - 38, 32});

    if (settingPassError) DrawText("[!] Passwords do not match.", (int)(rx + 14), (int)(py + 176), 13, COL_RED);

    if (DrawButton({rx + 14, py + 196, 150, 34}, "UPDATE PASSWORD")) {
        if (settingNewPass.len == 0) {
            toast.show("Password cannot be empty.", true);
        } else if (strcmp(settingNewPass.buf, settingConfirmPass.buf) != 0) {
            settingPassError = true;
        } else {
            settingPassError = false;
            settingNewPass.clear(); settingConfirmPass.clear();
            toast.show("Password updated successfully!");
        }
    }

    // ---- Bottom: System Info ----
    float iy = py + 260;
    DrawRoundedBox({px, iy, (float)(sw - 80), 130}, 0.08f, COL_PANEL2, COL_BORDER);
    DrawText("System Information", (int)(px + 14), (int)(iy + 12), 16, COL_HEADER);

    char medInfo[60];  sprintf(medInfo,  "Total Medicines   : %d", medCount);
    char patInfo[60];  sprintf(patInfo,  "Total Patients    : %d", patientCount);
    char presInfo[60]; sprintf(presInfo, "Total Prescriptions: %d", prescCount);
    char supInfo[60];  sprintf(supInfo,  "Total Suppliers   : %d", supplierCount);

    DrawText(medInfo,  (int)(px + 20), (int)(iy + 40), 13, COL_TEXT);
    DrawText(patInfo,  (int)(px + 20), (int)(iy + 60), 13, COL_TEXT);
    DrawText(presInfo, (int)(px + 20 + (sw - 80) / 2), (int)(iy + 40), 13, COL_TEXT);
    DrawText(supInfo,  (int)(px + 20 + (sw - 80) / 2), (int)(iy + 60), 13, COL_TEXT);
    DrawText("Version: 1.0  |  Rose Pharmacy Management System  |  Raylib GUI Edition",
             (int)(px + 20), (int)(iy + 96), 12, COL_SUBTEXT);

    if (DrawButton({20, btnY, 110, 36}, "BACK")) curScreen = SCR_ADMIN_DASH;
}

// ============================================================
// DRAW: ABOUT SCREEN
// ============================================================
void DrawAboutScreen() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), COL_BG);
    DrawHeader("ABOUT", "Rose Pharmacy Management System");

    int sw = GetScreenWidth(), sh = GetScreenHeight();
    float cx = sw / 2.0f;

    // Animated decorative circles
    float t = GetTime();
    DrawCircle((int)cx, sh / 2, (int)(160 + sinf(t) * 6), (Color){255, 210, 225, 40});
    DrawCircle((int)cx, sh / 2, (int)(110 + sinf(t + 1) * 5), (Color){255, 182, 193, 50});
    DrawCircle((int)cx, sh / 2, (int)( 65 + sinf(t + 2) * 4), (Color){219, 112, 147, 60});

    // Logo circle
    DrawCircle((int)cx, 140, 44, COL_ACCENT);
    DrawText("Rx", (int)(cx - 18), 120, 36, WHITE);

    DrawText("Rose Pharmacy", (int)(cx - MeasureText("Rose Pharmacy", 26) / 2), 198, 26, COL_HEADER);
    DrawText("Management System", (int)(cx - MeasureText("Management System", 16) / 2), 232, 16, COL_SUBTEXT);

    float fy = 270;
    DrawLine(sw / 4, (int)fy, 3 * sw / 4, (int)fy, COL_BORDER); fy += 16;

    struct { const char* label; const char* value; } info[] = {
        {"Version",     "1.0.0"},
        {"Framework",   "Raylib 5.0 (C++)"},
        {"Theme",       "Rose / Light Pink"},
        {"Author",      "Pharmacy Dev Team"},
        {"Platform",    "Cross-platform Desktop"},
        {"License",     "Educational Use"},
    };
    for (int i = 0; i < 6; i++) {
        int lw = MeasureText(info[i].label, 14);
        DrawText(info[i].label, (int)(cx - 180), (int)(fy + i * 28), 14, COL_TEXT);
        DrawText(":",           (int)(cx - 180 + lw + 4), (int)(fy + i * 28), 14, COL_ACCENT);
        DrawText(info[i].value, (int)(cx - 40), (int)(fy + i * 28), 14, COL_SUBTEXT);
    }

    fy += 180;
    DrawLine(sw / 4, (int)fy, 3 * sw / 4, (int)fy, COL_BORDER); fy += 16;
    DrawText("Features: Inventory Management | Patient Records | Prescriptions",
             (int)(cx - MeasureText("Features: Inventory Management | Patient Records | Prescriptions", 12) / 2),
             (int)fy, 12, COL_SUBTEXT);
    fy += 18;
    DrawText("Sales Counter | Supplier Directory | Alerts | Search | Settings",
             (int)(cx - MeasureText("Sales Counter | Supplier Directory | Alerts | Search | Settings", 12) / 2),
             (int)fy, 12, COL_SUBTEXT);

    if (DrawButton({(float)(sw / 2 - 60), (float)(sh - 60), 120, 36}, "BACK"))
        curScreen = SCR_ADMIN_DASH;
}

// ============================================================
// EXTENDED ADMIN DASHBOARD (replaces original)
// ============================================================
void DrawAdminDashExtended() {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), COL_BG);
    DrawHeader("ADMIN DASHBOARD", "Rose Pharmacy — Administrator");

    int sw = GetScreenWidth();

    // Compute stats
    int totalQty = 0, expired = 0, lowStock = 0;
    for (int i = 0; i < medCount; i++) {
        totalQty += meds[i].qty;
        if (meds[i].expired) expired++;
        if (meds[i].qty <= settingLowThreshold && !meds[i].expired) lowStock++;
    }

    // Top stat row
    struct { const char* l; int v; Color c; } stats[] = {
        {"Medicines",   medCount,      COL_ACCENT},
        {"Total Stock", totalQty,      COL_GREEN},
        {"Expired",     expired,       COL_RED},
        {"Low Stock",   lowStock,      COL_ORANGE},
        {"Patients",    patientCount,  COL_HEADER},
        {"Suppliers",   supplierCount, (Color){100,149,237,255}},
    };
    float statW = (sw - 50.0f) / 6.0f;
    for (int i = 0; i < 6; i++) {
        float sx = 20 + i * (statW + 2);
        DrawRoundedBox({sx, 74, statW, 60}, 0.15f, COL_PANEL, COL_BORDER);
        char vstr[16]; sprintf(vstr, "%d", stats[i].v);
        DrawText(vstr, (int)(sx + (statW - MeasureText(vstr, 22)) / 2), 82, 22, stats[i].c);
        DrawText(stats[i].l, (int)(sx + (statW - MeasureText(stats[i].l, 11)) / 2), 108, 11, COL_SUBTEXT);
    }

    // Navigation cards grid (3 x 3)
    struct { const char* icon; const char* label; const char* desc; Screen s; } cards[] = {
        {"[I]", "Inventory",      "View all medicines",       SCR_INVENTORY},
        {"[R]", "Restock",        "Update stock levels",      SCR_RESTOCK},
        {"[!]", "Alerts",         "Low stock / expired",      SCR_ALERTS},
        {"[P]", "Patients",       "Patient records",          SCR_PATIENTS},
        {"[Rx]","Prescriptions",  "Manage prescriptions",     SCR_PRESCRIPTIONS},
        {"[S]", "Suppliers",      "Supplier directory",       SCR_SUPPLIERS},
        {"[?]", "Search",         "Find medicines",           SCR_SEARCH},
        {"[*]", "Settings",       "System configuration",     SCR_SETTINGS},
        {"[@]", "About",          "System information",       SCR_ABOUT},
    };

    float gridTop = 150.0f;
    float cardW = (sw - 60.0f) / 3.0f - 4;
    float cardH = 78;
    for (int i = 0; i < 9; i++) {
        float gx = 20 + (i % 3) * (cardW + 10);
        float gy = gridTop + (i / 3) * (cardH + 10);
        bool hov = CheckCollisionPointRec(GetMousePosition(), {gx, gy, cardW, cardH});
        DrawRoundedBox({gx, gy, cardW, cardH}, 0.12f, hov ? COL_PANEL : COL_PANEL2, COL_BORDER);
        if (hov) DrawRectangleRoundedLines({gx, gy, cardW, cardH}, 0.12f, 8, 2.2f, COL_ACCENT);

        DrawCircle((int)(gx + 26), (int)(gy + 26), 16, COL_ACCENT2);
        DrawText(cards[i].icon,  (int)(gx + 8), (int)(gy + 16), 12, COL_HEADER);
        DrawText(cards[i].label, (int)(gx + 50), (int)(gy + 12), 16, COL_TEXT);
        DrawText(cards[i].desc,  (int)(gx + 50), (int)(gy + 34), 12, COL_SUBTEXT);
        if (hov) DrawText("->", (int)(gx + cardW - 24), (int)(gy + 30), 14, COL_ACCENT);

        if (hov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) curScreen = cards[i].s;
    }

    if (DrawButton({20, (float)(GetScreenHeight() - 54), 110, 36}, "LOGOUT"))
        curScreen = SCR_MAIN;
}

// ============================================================
// MAIN
// ============================================================
int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(900, 640, "Rose Pharmacy Management System");
    SetTargetFPS(60);

    initMedicines();
    initDefaultData();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(COL_BG);

        switch (curScreen) {
            case SCR_MAIN:          DrawMainMenu();             break;
            case SCR_ADMIN_LOGIN:   DrawAdminLogin();           break;
            case SCR_ADMIN_DASH:    DrawAdminDashExtended();    break;
            case SCR_INVENTORY:     DrawInventory(true);        break;
            case SCR_RESTOCK:       DrawRestock();              break;
            case SCR_ALERTS:        DrawAlerts();               break;
            case SCR_SALES:         DrawSalesScreen();          break;
            case SCR_INVOICE:       DrawInvoice();              break;
            case SCR_PATIENTS:      DrawPatientsScreen();       break;
            case SCR_PRESCRIPTIONS: DrawPrescriptionsScreen();  break;
            case SCR_SUPPLIERS:     DrawSuppliersScreen();      break;
            case SCR_SEARCH:        DrawSearchScreen();         break;
            case SCR_SETTINGS:      DrawSettingsScreen();       break;
            case SCR_ABOUT:         DrawAboutScreen();          break;
            default: break;
        }

        toast.draw(GetScreenWidth());
        EndDrawing();
    }

    CloseWindow();
    return 0;
}