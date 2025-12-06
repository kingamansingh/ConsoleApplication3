#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <sstream> 
#include <conio.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

#include "network.h"

using namespace std;

// Enable ANSI escape codes in Windows CMD
void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    SetConsoleOutputCP(CP_UTF8);
}

// Clear screen
static void clearScreen() {
    std::system("cls || clear");//cout << "\033[2J\033[H" << flush;
}

// Set cursor position (x=column, y=row)
static void setCursor(int x, int y) {
    cout << "\033[" << x << ";" << y << "H" << flush;
}

static void clearArea(int len, int hit) {
    string block = "";
    for (int i = 0; i < hit; i++) {
        string s = "";
        block += "\033[s" + s.assign(len, ' ') + "\033[u\033[1B";
    }
    cout << block << flush;
}

static void message(const string& text) {
    setCursor(45, (211 - text.length())/2);
    cout << text << flush;
}

// Write colored text at specific position
static void writeColorAt(int x, int y, const string& text, int colorCode) {
    setCursor(x, y);
    cout << "\033[" << colorCode << "m" << text << "\033[0m" << flush;
}

 //Play sound file
static void playSound(const string& filename, bool async = true) {
    wstring wideFilename(filename.begin(), filename.end());

    if (async) {
        PlaySound(wideFilename.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    }
    else {
        PlaySound(wideFilename.c_str(), NULL, SND_FILENAME | SND_SYNC);
    }
}

// Stop currently playing sound
static void stopSound() {
    PlaySound(NULL, NULL, 0);
}

static void clearInputBuffer() {
    while (_kbhit()) {
        _getch();
    }
}

static char waitForKey() { return _getch(); }

class AniMan {
public:
    // Load ANSI art file into string
    std::string loadFrame(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file: " << filename << std::endl;
            return "\x1b[31m[ERROR: FILE NOT FOUND: " + filename + "]\x1b[0m\n";
        }

        std::string content(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );
        return content;
    }
    std::vector<std::string> loaded_frames;

    AniMan(const std::vector <std::string> FRAME_FILENAMES) {
        for (const auto& filename : FRAME_FILENAMES) {
            loaded_frames.push_back(loadFrame(filename));// Load all frames into memory first for fast access during animation
        }

        if (loaded_frames.empty() || (loaded_frames.size() == 1 && loaded_frames[0].find("[ERROR:") != std::string::npos)) {
            std::cerr << "Fatal Error: No usable frames were loaded. Animation aborted." << std::endl;
            return;
        }
        //std::system("cls || clear");
    }
    int l, h, a, b;

    void out(int delay=0, int len = 0, int hit = 0, int x=0, int y=0) {
        l = len;
        h = hit;
		a = x;
		b = y;
        //const int DURATION_SECONDS = 10; // Animation duration if used//auto start_time = std::chrono::steady_clock::now();
        int frame_index = 0;
        try {//while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(DURATION_SECONDS));
            while (frame_index < loaded_frames.size()) {
                const std::string& current_frame = loaded_frames[frame_index % loaded_frames.size()];// Get the current frame content from the loaded vector

                if(x) setCursor(x, y);
                if (loaded_frames.size() > 1) {
					clearArea(len, hit);
                    setCursor(x, y);
                }
                std::cout << current_frame;
                std::cout.flush();// This forces the content to render on the console buffer instantly.
                frame_index++;
                // 5. Wait for the specified delay
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            }
        }
        catch (...) {
            // Simple error handling for interruption
            std::cout << "\nAnimation interrupted." << std::endl;
        }
    }


    void clearthis() const {
		setCursor(a, b);
        clearArea(l, h);
	}

};

AniMan h1({ "heart.utf8ans" }), gun({ "heartblack.utf8ans" }), shootp2({ "p1_fire1.utf8ans", "p1_fire2.utf8ans", "p2_smoke1.utf8ans", "p2_smoke2.utf8ans", "p2_smoke3.utf8ans", "p2_smoke4.utf8ans" }), shootp1({ "p2_fire1.utf8ans", "p2_fire2.utf8ans", "p1_smoke1.utf8ans", "p1_smoke2.utf8ans" , "p1_smoke3.utf8ans" , "p1_smoke4.utf8ans" });
AniMan start({ "start.utf8ans" }), live({ "live.utf8ans" }), blank({ "blank.utf8ans" }), p1self({ "p1self1.utf8ans", "p1self2.utf8ans", "p1self3.utf8ans", "p1self4.utf8ans", "p1self5.utf8ans"}), p2self({"p2self2.utf8ans", "p2self2.utf8ans" , "p2self3.utf8ans" , "p2self4.utf8ans" , "p2self5.utf8ans"});
AniMan p1({ "p1.utf8ans" }), p2({ "p2.utf8ans" }), gun1({ "p1gun.utf8ans" }), gun2({ "p2gun.utf8ans" }), gun_1({ "gun_1.utf8ans" }), gun_2({ "gun_2.utf8ans" }), you({ "you.utf8ans" }), mog({"mogambo.utf8ans "});
AniMan skip({ "skip.utf8ans" }), x2({ "2xcard.utf8ans" }), hp1({ "+1hp.utf8ans" }), Switch({ "switch.utf8ans" }), look({ "sneakpeek.utf8ans" }), eject({ "ejectshell.utf8ans" }), card({ "card.utf8ans" });

// Game Items
enum ItemType {
    MAGNIFYING_GLASS,
    CIGARETTE,
    HANDCUFFS,
    BEER,
    SAW,
    SWITCH,
    NONE
};

struct Item {
    ItemType type;
    std::string name;
    std::string description;
};


// Game State
class Game {
private:

    int playerHP;
    int mogamboHP;
    int maxHP;
    int maxItem;
    std::vector<bool> shells;
    int currentShell;
    std::vector<Item> playerItems;
    std::vector<Item> mogamboItems;
    bool playerTurn;
    bool mogamboSkipTurn;
    bool sawActive;
    int round;

	std::mt19937 rng;

    int pickWeighted(const vector<int>& weights) {
        int sum = 0;
        for (int w : weights) sum += w;
        int r = rng() % sum;
        int accum = 0;
        for (int i = 0; i < weights.size(); i++) {
            accum += weights[i];
            if (r < accum) return i;
        }
        return weights.size() - 1;
    }

    void initializeItems() {
        std::vector<ItemType> availableItems = {
            MAGNIFYING_GLASS, CIGARETTE, HANDCUFFS, BEER, SAW, SWITCH
        };

        std::shuffle(availableItems.begin(), availableItems.end(), rng);

        int playerItemCount = 2 /*+ (rng() % 3)*/;
        for (int i = 0; i < playerItemCount && i < availableItems.size(); i++) {
			if (playerItems.size() == maxItem) break;
            playerItems.push_back(createItem(availableItems[i]));
        }

        int mogamboItemCount = 2 /*+ (rng() % 3)*/;
        std::shuffle(availableItems.begin(), availableItems.end(), rng);
        for (int i = 0; i < mogamboItemCount && i < availableItems.size(); i++) {
			if (mogamboItems.size() == maxItem) break;
            mogamboItems.push_back(createItem(availableItems[i]));
        }
    }

    Item createItem(ItemType type) {
        Item item;
        item.type = type;
        switch (type) {
        case MAGNIFYING_GLASS:
            item.name = "Magnifying Glass";
            item.description = "Reveal current shell";
            break;
        case CIGARETTE:
            item.name = "Cigarette";
            item.description = "Restore 1 HP";
            break;
        case HANDCUFFS:
            item.name = "Handcuffs";
            item.description = "Skip opponent's turn";
            break;
        case SWITCH:
            item.name = "Switch";
            item.description = "Change shell polarity";
            break;
        case BEER:
            item.name = "Beer";
            item.description = "Eject current shell";
            break;
        case SAW:
            item.name = "Saw";
            item.description = "Double damage";
            break;
        default:
            break;
        }
        return item;
    }

    void loadShells() {
        shells.clear();
        currentShell = 0;

        vector<int> totalWeights = { 4,8,10,8,4,2,1 };
        int tIndex = pickWeighted(totalWeights);
        int totalShells = tIndex + 2;


        vector<int> liveWeights = { 10,8,5,2,1 };
        int lIndex = pickWeighted(liveWeights);
        int liveShells = lIndex + 1;
        if (liveShells >= totalShells) liveShells = totalShells - 1;

        int blankShells = totalShells - liveShells;

        for (int i = 0; i < blankShells; i++) shells.push_back(false);
        for (int i = 0; i < liveShells; i++) shells.push_back(true);

        setCursor(25,75);
		for (int i = 0; i < shells.size(); i++) shells[i] ? live.out(0, 9, 8) : blank.out(0, 9, 8);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        std::shuffle(shells.begin(), shells.end(), rng);
        std::system("cls || clear");
        resetmogamboKnowledge();
        displayTable();
		//waitForKey();
    }

    void displayTable() {
		setCursor(1, 1);// Player HP
        for (int i = 0; i < maxHP; i++) (i < playerHP) ? h1.out() : gun.out();

        setCursor(1, (211 - (maxHP * 14)));// mogambo HP
        for (int i = 0; i < maxHP; i++) (i < (maxHP - mogamboHP)) ? gun.out() : h1.out();
        you.out(0, 15, 4, 10, 40), mog.out(0, 40, 4, 10, 150);
    }

    void displayItems() { 
        setCursor(41, 1);//playerItems
        for (int i = 0; i < maxItem; i++) (i < playerItems.size()) ? itemmap(playerItems[i]) : card.out();
        setCursor(41, 117);//mogamboItems
        for (int i = 0; i < maxItem; i++) (i < (maxItem - mogamboItems.size())) ? card.out() : itemmap(mogamboItems[i - (maxItem - mogamboItems.size())]);
    }

    void playerAction() {
        start.out(0, 0, 0, 1, 1);
        displayTable();
        displayItems();
        gun1.out(0, 45, 8, 30, 60);
        char choice = waitForKey();
        if (choice >= '1' && choice <= '6') {
            int itemIndex = choice - '1';
            if (itemIndex < playerItems.size()) {
                useItem(playerItems[itemIndex], true);
                playerItems.erase(playerItems.begin() + itemIndex);
                playerAction();
                return;
            }

        }
        else if (choice == 75) {
            gun1.clearthis();
            shootSelf(true);
        }
        else if (choice == 77) {
            shootOpponent(true);
        }
    }

    // Improved mogambo decision making
    void mogamboActionSmart() {
        start.out(0, 0, 0, 1, 1);
        displayTable();
        displayItems();
        cout << "mogambo's turn..." << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        // Use items strategically
        if (mogamboUseItemsSmart()) {
            mogamboActionSmart(); // Continue turn after using item
            return;
        }

        // Decide whether to shoot self or opponent based on knowledge
        if (mogamboKnowledge.knowsCurrentShell) {
            if (mogamboKnowledge.currentShellIsLive) {
                // We know it's live, shoot opponent
                shootOpponent(false);
                mogamboKnowledge.knowsCurrentShell = false; // Reset after shot
            }
            else {
                // We know it's blank, shoot self to keep turn
                shootSelf(false);
                mogamboKnowledge.knowsCurrentShell = false; // Reset after shot
            }
        }
        else {
            // Don't know current shell, make probabilistic decision
            int liveCount = 0;
            int totalRemaining = 0;

            for (int i = currentShell; i < shells.size(); i++) {
                if (shells[i]) liveCount++;
                totalRemaining++;
            }

            // If more blanks than live shells remaining, shoot self
            // Otherwise shoot opponent (safer play)
            if (totalRemaining > 0 && liveCount < totalRemaining / 2) {
                shootSelf(false);
            }
            else {
                shootOpponent(false);
            }
        }
		clearInputBuffer();
    }

    void itemmap(const Item& item) {
        switch (item.type) {
        case MAGNIFYING_GLASS:
            look.out();
            break;

        case CIGARETTE:
            hp1.out();
            break;

        case HANDCUFFS:
            skip.out();
            break;

        case BEER:
            eject.out();
            break;

        case SAW:
            x2.out();
            break;

        case SWITCH:
            Switch.out();
            break;

        default:
            break;
        }
    }

    // mogambo AI decision making
    struct mogamboKnowledge {
        bool knowsCurrentShell = false;
        bool currentShellIsLive = false;
    };

    mogamboKnowledge mogamboKnowledge;

    // Reset mogambo knowledge when new shells are loaded
    void resetmogamboKnowledge() {
        mogamboKnowledge.knowsCurrentShell = false;
        mogamboKnowledge.currentShellIsLive = false;
    }

    bool mogamboUseItemsSmart() {
        if (mogamboItems.empty()) return false;

        // Priority 1: Use magnifying glass if we don't know the current shell
        if (!mogamboKnowledge.knowsCurrentShell) {
            for (int i = 0; i < mogamboItems.size(); i++) {
                if (mogamboItems[i].type == MAGNIFYING_GLASS) {
                    mogamboKnowledge.knowsCurrentShell = true;
                    mogamboKnowledge.currentShellIsLive = shells[currentShell];
                    useItem(mogamboItems[i], false);
                    mogamboItems.erase(mogamboItems.begin() + i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return true; // Continue with strategy after knowing
                }
            }
        }

        // Priority 2: If we know it's a LIVE shell and shooting opponent
        if (mogamboKnowledge.knowsCurrentShell && mogamboKnowledge.currentShellIsLive) {
            // Use saw for double damage before shooting opponent
            for (int i = 0; i < mogamboItems.size(); i++) {
                if (mogamboItems[i].type == SAW && !sawActive) {
                    useItem(mogamboItems[i], false);
                    mogamboItems.erase(mogamboItems.begin() + i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return true;
                }
            }
            return false; // Ready to shoot opponent
        }

        // Priority 3: If we know it's a BLANK shell
        if (mogamboKnowledge.knowsCurrentShell && !mogamboKnowledge.currentShellIsLive) {
            // Use handcuffs before shooting self with blank (get extra turn)
            for (int i = 0; i < mogamboItems.size(); i++) {
                if (mogamboItems[i].type == HANDCUFFS) {
                    useItem(mogamboItems[i], false);
                    mogamboItems.erase(mogamboItems.begin() + i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return true;
                }
            }
            return false; // Ready to shoot self with blank
        }

        // Priority 4: If low HP, try to heal
        if (mogamboHP <= maxHP / 2) {
            for (int i = 0; i < mogamboItems.size(); i++) {
                if (mogamboItems[i].type == CIGARETTE && mogamboHP < maxHP) {
                    useItem(mogamboItems[i], false);
                    mogamboItems.erase(mogamboItems.begin() + i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return true;
                }
            }
        }

        // Priority 5: If uncertain and have beer, eject shell (risky shell management)
        if (!mogamboKnowledge.knowsCurrentShell && shells.size() - currentShell > 2) {
            for (int i = 0; i < mogamboItems.size(); i++) {
                if (mogamboItems[i].type == BEER) {
                    useItem(mogamboItems[i], false);
                    mogamboItems.erase(mogamboItems.begin() + i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    mogamboKnowledge.knowsCurrentShell = false; // Reset knowledge
                    return true;
                }
            }
        }

        // Priority 6: Use switch if we know it's live and want to shoot self
        if (mogamboKnowledge.knowsCurrentShell && mogamboKnowledge.currentShellIsLive) {
            for (int i = 0; i < mogamboItems.size(); i++) {
                if (mogamboItems[i].type == SWITCH) {
                    useItem(mogamboItems[i], false);
                    mogamboItems.erase(mogamboItems.begin() + i);
                    mogamboKnowledge.currentShellIsLive = false; // Now it's blank
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return true;
                }
            }
        }

        return false;
    }

    void useItem(const Item& item, bool isPlayer) {
        std::string user = isPlayer ? "You" : "mogambo";
        cout << user << " used " << item.name << endl;

        switch (item.type) {
        case MAGNIFYING_GLASS:
            if (currentShell < shells.size()) {
                std::string shellType = shells[currentShell] ? "LIVE" : "BLANK";
                if (isPlayer) {
                    cout << "Current shell: " << shellType << endl;
                }
                else {
                    cout << "mogambo examines the shell..." << endl;
                }
            }
            else {
                cout << "No shells left to examine!" << endl;
            }
            break;

        case CIGARETTE:
            if (isPlayer && playerHP < maxHP) {
                playerHP++;
                cout << "Healed 1 HP" << endl; 
            }
            else if (!isPlayer && mogamboHP < maxHP) {
                mogamboHP++;
                cout << "mogambo healed 1 HP" << endl;
            }
            break;

        case HANDCUFFS:
            if (isPlayer) {
                mogamboSkipTurn = true;
                //cout << "mogambo's turn will be skipped!" << endl;
            }
            else {
                // mogambo can't really handcuff player in current implementation
                // but you could add playerSkipTurn if you want
                cout << "mogambo used handcuffs!" << endl;
            }
            break;

        case BEER:
            if (currentShell < shells.size()) {
                bool wasLive = shells[currentShell];
                currentShell++;
                std::string ejected = wasLive ? "LIVE" : "BLANK";
                cout << "Ejected " << ejected << " shell" << endl;
            }
            else {
                cout << "No shells to eject!" << endl;
            }
            break;

        case SAW:
            sawActive = true;
            cout << "Next shot will deal DOUBLE DAMAGE!" << endl;
            break;

        case SWITCH:
            if (currentShell < shells.size()) {
                shells[currentShell] = !shells[currentShell];
                cout << "Inverted the current shell!" << endl;
            }
            else {
                cout << "No shell to switch!" << endl;
            }
            break;

        default:
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    void shootSelf(bool isPlayer) {
        isPlayer ? gun_1.out(0, 45, 8, 25, 60) : gun_2.out(0, 45, 8, 25, 108);
        setCursor(1, 1);
        if (currentShell >= shells.size()) {
            return;
        }
        std::string shooter = isPlayer ? "You" : "mogambo";
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        bool isLive = shells[currentShell];
        currentShell++;

        if (isLive) {
            playSound("shotgun.wav", true);
            isPlayer ? p1self.out(100, 42, 20, 21, 18) : p2self.out(100, 42, 20, 21, 153);
            isPlayer ? p1.out(0, 41, 20, 21, 18) : p2.out(0, 41, 20, 21, 153);
            isPlayer ? gun_1.clearthis() : gun_2.clearthis();

            int damage = sawActive ? 2 : 1;
            if (isPlayer) {
                playerHP -= damage;
                setCursor(1, 1);
                for (int i = 0; i < maxHP; i++) (i < playerHP) ? h1.out() : h1.clearthis();
            }
            else {
                mogamboHP -= damage;
                setCursor(1, (211 - (maxHP * 14)));
                for (int i = 0; i < maxHP; i++) (i < (maxHP - mogamboHP)) ? gun.out() : h1.out();
            }
            sawActive = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            playerTurn = !playerTurn;
        }
        else {
            playSound("empty.wav", true);
            sawActive = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
        gun_1.clearthis();
        gun_2.clearthis();
    }

    void shootOpponent(bool isPlayer) {
        isPlayer ? gun1.out(0, 45, 8, 30, 60) : gun2.out(0, 45, 8, 30, 108);
        setCursor(1, 1);
        
        if (currentShell >= shells.size()) return;

        std::string shooter = isPlayer ? "You" : "mogambo";
        std::string target = isPlayer ? "mogambo" : "you";
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        bool isLive = shells[currentShell];
        currentShell++;

        if (isLive) {
            playSound("shotgun.wav", true);
            isPlayer ? shootp2.out(5, 93, 28, 14, 118) : shootp1.out(5, 93, 28, 14, 1);
			isPlayer ? shootp2.clearthis() : shootp1.clearthis();
            int damage = sawActive ? 2 : 1;
            if (isPlayer) {
                mogamboHP -= damage;
                setCursor(1, 1);
                for (int i = 0; i < maxHP; i++) (i < playerHP) ? h1.out() : h1.clearthis();
            }
            else {
                playerHP -= damage;
                setCursor(1, (211 - (maxHP * 14)));
                for (int i = 0; i < maxHP; i++) (i < (maxHP - mogamboHP)) ? gun.out() : h1.out();
            }
            sawActive = false;
        }
        else {
            playSound("empty.wav", true);
            sawActive = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        playerTurn = !playerTurn;
        isPlayer ? p2.out(0, 41, 20, 21, 153) : p1.out(0, 41, 20, 21, 18);
        gun2.clearthis();
        gun1.clearthis();

    }

public:
    Game() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
        playerHP = 3;
        mogamboHP = 3;
        maxHP = 3;
        maxItem = 6;
        currentShell = 0;
        playerTurn = true;
        mogamboSkipTurn = false;
        sawActive = false;
        round = 1;
    }

    void showMenu() {
        std::system("cls || clear");
		cout << "Press any key to start" << endl;
		cout << "ESC to exit" << endl;

        char key = waitForKey();
        if (key == 27) exit(0);
        std::system("cls || clear");
    }

    void run() {
        showMenu();
        while (true) {
            loadShells();
			initializeItems();
            while (playerHP > 0 && mogamboHP > 0 && currentShell < shells.size()) {
                if (playerTurn) {
                    playerAction();
                }
                else {
                    if (mogamboSkipTurn) {
                        clearScreen();
                        
						cout << "mogambo is handcuffed! Turn skipped." << endl;
                        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                        mogamboSkipTurn = false;
                        playerTurn = true;
                    }
                    else {
                        mogamboActionSmart();
                    }
                }
            }
            if (playerHP <= 0) {
                gameOver(false);
                break;
            }
            else if (mogamboHP <= 0) {
                gameOver(true);
                break;
            }

            if (currentShell >= shells.size()) {
                std::system("cls || clear");

                cout << "Round " << round << " Complete!" << endl;
                round++;
				cout << "Press any key to continue..." << endl;
                waitForKey();
                std::system("cls || clear");
            }
        }
    }

    void gameOver(bool playerWon) {
        clearScreen();
        if (playerWon) cout << "YOU WIN!\nMOGAMBO FUSS HUA !" << endl;
        else cout << "YOU DIED\nMOGAMBO KHUSH HUA !" << endl;
		cout << "Press any key to exit..." << endl;
    }
};
//AniMan trial({ "mogambo.utf8ans" }), flash({"flash.utf8ans"});
static void setConsoleSize(int width, int height)
{
    HWND console = GetConsoleWindow();
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // 1. Set the buffer size first
    COORD bufferSize;
    bufferSize.X = width;
    bufferSize.Y = height;
    SetConsoleScreenBufferSize(hOut, bufferSize);

    // 2. Set the window size
    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = width - 1;
    windowSize.Bottom = height - 1;

    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
}

static void removeScrollbars() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hOut, &csbi);

    COORD newSize;
    newSize.X = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    newSize.Y = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    SetConsoleScreenBufferSize(hOut, newSize);
}

static void lockConsoleResize() {
    HWND console = GetConsoleWindow();
    LONG style = GetWindowLong(console, GWL_STYLE);
    style &= ~WS_SIZEBOX;
    style &= ~WS_MAXIMIZEBOX;
    SetWindowLong(console, GWL_STYLE, style);
}


class MultiplayerGame {
private:
    SOCKET socket;
    bool isHost;

    // Game state
    int playerHP;
    int opponentHP;
    int maxHP;
    int maxItem;
    std::vector<bool> shells;
    int currentShell;
    std::vector<Item> playerItems;
    std::vector<Item> opponentItems;
    bool playerTurn;
    bool opponentSkipTurn;
    bool sawActive;
    int round;
    std::mt19937 rng;

    GameMessage currentMsg;

    int pickWeighted(const vector<int>& weights) {
        int sum = 0;
        for (int w : weights) sum += w;
        int r = rng() % sum;
        int accum = 0;
        for (int i = 0; i < weights.size(); i++) {
            accum += weights[i];
            if (r < accum) return i;
        }
        return weights.size() - 1;
    }

    Item createItem(ItemType type) {
        Item item;
        item.type = type;
        switch (type) {
        case MAGNIFYING_GLASS:
            item.name = "Magnifying Glass";
            item.description = "Reveal current shell";
            break;
        case CIGARETTE:
            item.name = "Cigarette";
            item.description = "Restore 1 HP";
            break;
        case HANDCUFFS:
            item.name = "Handcuffs";
            item.description = "Skip opponent's turn";
            break;
        case SWITCH:
            item.name = "Switch";
            item.description = "Change shell polarity";
            break;
        case BEER:
            item.name = "Beer";
            item.description = "Eject current shell";
            break;
        case SAW:
            item.name = "Saw";
            item.description = "Double damage";
            break;
        default:
            break;
        }
        return item;
    }

    void initializeItems() {
        std::vector<ItemType> availableItems = {
            MAGNIFYING_GLASS, CIGARETTE, HANDCUFFS, BEER, SAW, SWITCH
        };

        std::shuffle(availableItems.begin(), availableItems.end(), rng);

        playerItems.clear();
        opponentItems.clear();

        int playerItemCount = 2;
        for (int i = 0; i < playerItemCount && i < availableItems.size(); i++) {
            if (playerItems.size() == maxItem) break;
            playerItems.push_back(createItem(availableItems[i]));
        }

        int opponentItemCount = 2;
        std::shuffle(availableItems.begin(), availableItems.end(), rng);
        for (int i = 0; i < opponentItemCount && i < availableItems.size(); i++) {
            if (opponentItems.size() == maxItem) break;
            opponentItems.push_back(createItem(availableItems[i]));
        }
    }

    void loadShells() {
        shells.clear();
        currentShell = 0;

        vector<int> totalWeights = { 4,8,10,8,4,2,1 };
        int tIndex = pickWeighted(totalWeights);
        int totalShells = tIndex + 2;

        vector<int> liveWeights = { 10,8,5,2,1 };
        int lIndex = pickWeighted(liveWeights);
        int liveShells = lIndex + 1;
        if (liveShells >= totalShells) liveShells = totalShells - 1;

        int blankShells = totalShells - liveShells;

        for (int i = 0; i < blankShells; i++) shells.push_back(false);
        for (int i = 0; i < liveShells; i++) shells.push_back(true);

        setCursor(25, 75);
        for (int i = 0; i < shells.size(); i++) shells[i] ? live.out(0, 9, 8) : blank.out(0, 9, 8);
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        std::shuffle(shells.begin(), shells.end(), rng);
        std::system("cls || clear");
        displayTable();
    }

    void displayTable() {
        setCursor(1, 1);
        for (int i = 0; i < maxHP; i++) (i < playerHP) ? h1.out() : gun.out();

        setCursor(1, (211 - (maxHP * 14)));
        for (int i = 0; i < maxHP; i++) (i < (maxHP - opponentHP)) ? gun.out() : h1.out();

        if (isHost) {
            you.out(0, 15, 4, 10, 40);
            mog.out(0, 40, 4, 10, 150);
        }
        else {
            mog.out(0, 40, 4, 10, 40);
            you.out(0, 15, 4, 10, 150);
        }
    }

    void displayItems() {
        setCursor(41, 1);
        for (int i = 0; i < maxItem; i++) {
            if (i < playerItems.size()) {
                itemmap(playerItems[i]);
            }
            else {
                card.out();
            }
        }

        setCursor(41, 117);
        for (int i = 0; i < maxItem; i++) {
            if (i < (maxItem - opponentItems.size())) {
                card.out();
            }
            else {
                itemmap(opponentItems[i - (maxItem - opponentItems.size())]);
            }
        }
    }

    void itemmap(const Item& item) {
        switch (item.type) {
        case MAGNIFYING_GLASS:
            look.out();
            break;
        case CIGARETTE:
            hp1.out();
            break;
        case HANDCUFFS:
            skip.out();
            break;
        case BEER:
            eject.out();
            break;
        case SAW:
            x2.out();
            break;
        case SWITCH:
            Switch.out();
            break;
        default:
            break;
        }
    }

    void useItem(const Item& item, bool isPlayer) {
        std::string user = isPlayer ? "You" : "Opponent";

        switch (item.type) {
        case MAGNIFYING_GLASS:
            if (currentShell < shells.size()) {
                std::string shellType = shells[currentShell] ? "LIVE" : "BLANK";
                if (isPlayer) {
                    message("Current shell: " + shellType);
                }
            }
            break;

        case CIGARETTE:
            if (isPlayer && playerHP < maxHP) {
                playerHP++;
                message("Healed 1 HP");
            }
            else if (!isPlayer && opponentHP < maxHP) {
                opponentHP++;
                message("Opponent healed 1 HP");
            }
            break;

        case HANDCUFFS:
            if (isPlayer) {
                opponentSkipTurn = true;
            }
            break;

        case BEER:
            if (currentShell < shells.size()) {
                currentShell++;
                message("Ejected shell");
            }
            break;

        case SAW:
            sawActive = true;
            message("Next shot will deal DOUBLE DAMAGE!");
            break;

        case SWITCH:
            if (currentShell < shells.size()) {
                shells[currentShell] = !shells[currentShell];
                message("Inverted the current shell!");
            }
            break;

        default:
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    void shootSelf(bool isPlayer) {
        if (isPlayer) {
            gun_1.out(0, 45, 8, 25, 60);
        }
        else {
            gun_2.out(0, 45, 8, 25, 108);
        }

        setCursor(1, 1);

        if (currentShell >= shells.size()) {
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        bool isLive = shells[currentShell];
        currentShell++;

        if (isLive) {
            playSound("shotgun.wav", true);

            if (isPlayer) {
                p1self.out(100, 42, 20, 21, 18);
                p1.out(0, 41, 20, 21, 18);
                gun_1.clearthis();
            }
            else {
                p2self.out(100, 42, 20, 21, 153);
                p2.out(0, 41, 20, 21, 153);
                gun_2.clearthis();
            }

            int damage = sawActive ? 2 : 1;
            if (isPlayer) {
                playerHP -= damage;
                setCursor(1, 1);
                for (int i = 0; i < maxHP; i++) (i < playerHP) ? h1.out() : h1.clearthis();
            }
            else {
                opponentHP -= damage;
                setCursor(1, (211 - (maxHP * 14)));
                for (int i = 0; i < maxHP; i++) (i < (maxHP - opponentHP)) ? gun.out() : h1.out();
            }
            sawActive = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            playerTurn = !playerTurn;
        }
        else {
            playSound("empty.wav", true);
            sawActive = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
        gun_1.clearthis();
        gun_2.clearthis();
    }

    void shootOpponent(bool isPlayer) {
        if (isPlayer) {
            gun1.out(0, 45, 8, 30, 60);
        }
        else {
            gun2.out(0, 45, 8, 30, 108);
        }

        setCursor(1, 1);

        if (currentShell >= shells.size()) return;

        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        bool isLive = shells[currentShell];
        currentShell++;

        if (isLive) {
            playSound("shotgun.wav", true);

            if (isPlayer) {
                shootp2.out(5, 93, 28, 14, 118);
                shootp2.clearthis();
            }
            else {
                shootp1.out(5, 93, 28, 14, 1);
                shootp1.clearthis();
            }

            int damage = sawActive ? 2 : 1;
            if (isPlayer) {
                opponentHP -= damage;
                setCursor(1, (211 - (maxHP * 14)));
                for (int i = 0; i < maxHP; i++) (i < (maxHP - opponentHP)) ? gun.out() : h1.out();
            }
            else {
                playerHP -= damage;
                setCursor(1, 1);
                for (int i = 0; i < maxHP; i++) (i < playerHP) ? h1.out() : h1.clearthis();
            }
            sawActive = false;
        }
        else {
            playSound("empty.wav", true);
            sawActive = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        playerTurn = !playerTurn;

        if (isPlayer) {
            p2.out(0, 41, 20, 21, 153);
        }
        else {
            p1.out(0, 41, 20, 21, 18);
        }

        gun2.clearthis();
        gun1.clearthis();
    }

    void gameOver(bool won) {
        clearScreen();
        if (won) {
            cout << "YOU WIN!\nMOGAMBO FUSS HUA !" << endl;
        }
        else {
            cout << "YOU DIED\nMOGAMBO KHUSH HUA !" << endl;
        }
        cout << "Press any key to exit..." << endl;
        waitForKey();
    }

public:
    MultiplayerGame(bool host, SOCKET sock)
        : isHost(host), socket(sock), rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
        playerHP = 3;
        opponentHP = 3;
        maxHP = 3;
        maxItem = 6;
        currentShell = 0;
        playerTurn = host;
        opponentSkipTurn = false;
        sawActive = false;
        round = 1;
    }

    void run() {
        if (isHost) {
            runHost();
        }
        else {
            runClient();
        }
    }

private:
    void runHost() {
        clearScreen();
        message("Starting game as HOST...");
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        // Send initial game start message
        GameMessage startMsg;
        startMsg.messageType = MSG_GAME_START;
        NetworkHelper::sendMessage(socket, startMsg);

        // Main game loop
        while (playerHP > 0 && opponentHP > 0) {
            if (currentShell >= shells.size()) {
                loadShells();
                initializeItems();
                sendGameState();
            }

            if (playerTurn) {
                hostTurn();
            }
            else {
                if (opponentSkipTurn) {
                    clearScreen();
                    message("Opponent is handcuffed! Turn skipped.");
                    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                    opponentSkipTurn = false;
                    playerTurn = true;
                    sendGameState();
                }
                else {
                    clientTurnHost();
                }
            }

            if (playerHP <= 0 || opponentHP <= 0) {
                sendGameOver();
                break;
            }
        }

        gameOver(playerHP > 0);
    }

    void runClient() {
        clearScreen();
        message("Connecting to HOST...");

        // Wait for game start
        GameMessage msg;
        if (!NetworkHelper::receiveMessage(socket, msg)) {
            message("Connection failed!");
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            return;
        }

        message("Game starting!");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Main game loop
        while (true) {
            if (!NetworkHelper::receiveMessage(socket, currentMsg)) {
                message("Connection lost!");
                break;
            }

            if (currentMsg.messageType == MSG_GAME_OVER) {
                bool won = (currentMsg.clientHP > 0);
                gameOver(won);
                break;
            }

            updateFromMessage(currentMsg);
            displayTable();
            displayItems();

            if (!playerTurn) {
                clientTurn();
            }

            if (playerHP <= 0 || opponentHP <= 0) {
                break;
            }
        }
    }

    void hostTurn() {
        start.out(0, 0, 0, 1, 1);
        displayTable();
        displayItems();
        gun1.out(0, 45, 8, 30, 60);

        char choice = waitForKey();

        if (choice >= '1' && choice <= '6') {
            int itemIndex = choice - '1';
            if (itemIndex < playerItems.size()) {
                useItem(playerItems[itemIndex], true);
                playerItems.erase(playerItems.begin() + itemIndex);
                sendGameState();
                hostTurn();
                return;
            }
        }
        else if (choice == 75) {
            gun1.clearthis();
            shootSelf(true);
        }
        else if (choice == 77) {
            shootOpponent(true);
        }

        sendGameState();
    }

    void clientTurnHost() {
        start.out(0, 0, 0, 1, 1);
        displayTable();
        displayItems();

        message("OPPONENT'S TURN - Waiting...");

        GameMessage actionMsg;
        if (!NetworkHelper::receiveMessage(socket, actionMsg)) {
            message("Connection lost!");
            return;
        }

        if (actionMsg.actionType == NET_ACTION_SHOOT_SELF) {
            shootSelf(false);
        }
        else if (actionMsg.actionType == NET_ACTION_SHOOT_OPPONENT) {
            shootOpponent(false);
        }
        else if (actionMsg.actionType == NET_ACTION_USE_ITEM) {
            if (actionMsg.itemIndex < opponentItems.size()) {
                useItem(opponentItems[actionMsg.itemIndex], false);
                opponentItems.erase(opponentItems.begin() + actionMsg.itemIndex);
            }
        }

        sendGameState();
    }

    void clientTurn() {
        gun2.out(0, 45, 8, 30, 108);
        char choice = waitForKey();

        GameMessage actionMsg;
        actionMsg.messageType = MSG_ACTION;

        if (choice >= '1' && choice <= '6') {
            int itemIndex = choice - '1';
            if (itemIndex < playerItems.size()) {
                actionMsg.actionType = NET_ACTION_USE_ITEM;
                actionMsg.itemIndex = itemIndex;
                NetworkHelper::sendMessage(socket, actionMsg);
                return;
            }
        }
        else if (choice == 75) {
            actionMsg.actionType = NET_ACTION_SHOOT_SELF;
        }
        else if (choice == 77) {
            actionMsg.actionType = NET_ACTION_SHOOT_OPPONENT;
        }

        NetworkHelper::sendMessage(socket, actionMsg);
    }

    void sendGameState() {
        GameMessage msg;
        msg.messageType = MSG_GAME_STATE;
        msg.hostHP = playerHP;
        msg.clientHP = opponentHP;
        msg.maxHP = maxHP;
        msg.hostTurn = playerTurn;
        msg.sawActive = sawActive;
        msg.skipTurn = opponentSkipTurn;
        msg.round = round;
        msg.totalShells = shells.size();
        msg.currentShell = currentShell;

        for (int i = 0; i < shells.size() && i < 10; i++) {
            msg.shells[i] = shells[i];
        }

        msg.hostItemCount = playerItems.size();
        msg.clientItemCount = opponentItems.size();
        for (int i = 0; i < playerItems.size() && i < 8; i++) {
            msg.hostItems[i] = playerItems[i].type;
        }
        for (int i = 0; i < opponentItems.size() && i < 8; i++) {
            msg.clientItems[i] = opponentItems[i].type;
        }

        NetworkHelper::sendMessage(socket, msg);
    }

    void updateFromMessage(const GameMessage& msg) {
        if (isHost) {
            playerHP = msg.hostHP;
            opponentHP = msg.clientHP;
        }
        else {
            playerHP = msg.clientHP;
            opponentHP = msg.hostHP;
        }

        maxHP = msg.maxHP;
        playerTurn = isHost ? msg.hostTurn : !msg.hostTurn;
        sawActive = msg.sawActive;
        round = msg.round;
        currentShell = msg.currentShell;

        shells.clear();
        for (int i = 0; i < msg.totalShells && i < 10; i++) {
            shells.push_back(msg.shells[i]);
        }

        playerItems.clear();
        opponentItems.clear();

        int myItemCount = isHost ? msg.hostItemCount : msg.clientItemCount;
        int oppItemCount = isHost ? msg.clientItemCount : msg.hostItemCount;
        int* myItems = isHost ? (int*)msg.hostItems : (int*)msg.clientItems;
        int* oppItems = isHost ? (int*)msg.clientItems : (int*)msg.hostItems;

        for (int i = 0; i < myItemCount && i < 8; i++) {
            playerItems.push_back(createItem((ItemType)myItems[i]));
        }
        for (int i = 0; i < oppItemCount && i < 8; i++) {
            opponentItems.push_back(createItem((ItemType)oppItems[i]));
        }
    }

    void sendGameOver() {
        GameMessage msg;
        msg.messageType = MSG_GAME_OVER;
        msg.hostHP = playerHP;
        msg.clientHP = opponentHP;
        NetworkHelper::sendMessage(socket, msg);
    }
};


// Network setup functions
SOCKET setupHostSocket(int port) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 1);

    cout << "Waiting for client on port " << port << "...\n";
    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    cout << "Client connected!\n";

    closesocket(serverSocket);
    return clientSocket;
}

SOCKET setupClientSocket(const string& hostname, const string& port) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    addrinfo hints, * result;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(hostname.c_str(), port.c_str(), &hints, &result);
    connect(clientSocket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);

    return clientSocket;
}

int main() {
    enableANSI();
    cout << "\033]0;ANSI ROULETTE\007";
    setConsoleSize(211, 49);
    lockConsoleResize();

    while (true) {
        std::system("cls || clear");
        cout << "=== BUCKSHOT ROULETTE ===\n\n";
        cout << "[1] Single Player (vs AI)\n";
        cout << "[2] Host Multiplayer Game\n";
        cout << "[3] Join Multiplayer Game\n";
        cout << "[ESC] Exit\n\n";
        cout << "Choose: ";

        char choice = waitForKey();

        if (choice == '1') {
            // Single player
            Game game;
            game.run();
        }
        else if (choice == '2') {
            // Host multiplayer
            cout << "\n\nStarting as HOST...\n";
            cout << "Make sure you have Pinggy tunnel running!\n";
            cout << "Listening on port 8000...\n\n";

            SOCKET clientSocket = setupHostSocket(8000);
            MultiplayerGame mpGame(true, clientSocket);
            mpGame.run();
            closesocket(clientSocket);
            WSACleanup();
        }
        else if (choice == '3') {
            // Join multiplayer
            cout << "\n\nJoining as CLIENT...\n";
            cout << "Enter hostname (from Pinggy): ";
            string hostname;
            cin >> hostname;
            cout << "Enter port: ";
            string port;
            cin >> port;
            cin.ignore();

            cout << "Connecting...\n";
            SOCKET serverSocket = setupClientSocket(hostname, port);
            MultiplayerGame mpGame(false, serverSocket);
            mpGame.run();
            closesocket(serverSocket);
            WSACleanup();
        }
        else if (choice == 27) {
            // ESC
            break;
        }
    }

    return 0;
}


//int main() {
//    enableANSI();
//    cout << "\033]0;ANSI ROULETTE\007"; // Set console title
//	setConsoleSize(211, 49);
//	lockConsoleResize();
//	//removeScrollbars();
//    Game game;
//    game.run();
//    return 0;
//}