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
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

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
    int dealerHP;
    int maxHP;
    int maxItem;
    std::vector<bool> shells;
    int currentShell;
    std::vector<Item> playerItems;
    std::vector<Item> dealerItems;
    bool playerTurn;
    bool dealerSkipTurn;
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

        int dealerItemCount = 2 /*+ (rng() % 3)*/;
        std::shuffle(availableItems.begin(), availableItems.end(), rng);
        for (int i = 0; i < dealerItemCount && i < availableItems.size(); i++) {
			if (dealerItems.size() == maxItem) break;
            dealerItems.push_back(createItem(availableItems[i]));
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
        resetDealerKnowledge();
        displayTable();
		//waitForKey();
    }

    void displayTable() {
		setCursor(1, 1);// Player HP
        for (int i = 0; i < maxHP; i++) (i < playerHP) ? h1.out() : gun.out();

        setCursor(1, (211 - (maxHP * 14)));// Dealer HP
        for (int i = 0; i < maxHP; i++) (i < (maxHP - dealerHP)) ? gun.out() : h1.out();
        you.out(0, 15, 4, 10, 40), mog.out(0, 40, 4, 10, 150);
    }

    void displayItems() { 
        setCursor(41, 1);//playerItems
        for (int i = 0; i < maxItem; i++) (i < playerItems.size()) ? itemmap(playerItems[i]) : card.out();
        setCursor(41, 117);//dealerItems
        for (int i = 0; i < maxItem; i++) (i < (maxItem - dealerItems.size())) ? card.out() : itemmap(dealerItems[i - (maxItem - dealerItems.size())]);
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

    // Improved dealer decision making
    void dealerActionSmart() {
        start.out(0, 0, 0, 1, 1);
        displayTable();
        displayItems();
        cout << "Dealer's turn..." << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        // Use items strategically
        if (dealerUseItemsSmart()) {
            dealerActionSmart(); // Continue turn after using item
            return;
        }

        // Decide whether to shoot self or opponent based on knowledge
        if (dealerKnowledge.knowsCurrentShell) {
            if (dealerKnowledge.currentShellIsLive) {
                // We know it's live, shoot opponent
                shootOpponent(false);
                dealerKnowledge.knowsCurrentShell = false; // Reset after shot
            }
            else {
                // We know it's blank, shoot self to keep turn
                shootSelf(false);
                dealerKnowledge.knowsCurrentShell = false; // Reset after shot
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

    // Dealer AI decision making
    struct DealerKnowledge {
        bool knowsCurrentShell = false;
        bool currentShellIsLive = false;
    };

    DealerKnowledge dealerKnowledge;

    // Reset dealer knowledge when new shells are loaded
    void resetDealerKnowledge() {
        dealerKnowledge.knowsCurrentShell = false;
        dealerKnowledge.currentShellIsLive = false;
    }

    bool dealerUseItemsSmart() {
        if (dealerItems.empty()) return false;

        // Priority 1: Use magnifying glass if we don't know the current shell
        if (!dealerKnowledge.knowsCurrentShell) {
            for (int i = 0; i < dealerItems.size(); i++) {
                if (dealerItems[i].type == MAGNIFYING_GLASS) {
                    dealerKnowledge.knowsCurrentShell = true;
                    dealerKnowledge.currentShellIsLive = shells[currentShell];
                    useItem(dealerItems[i], false);
                    dealerItems.erase(dealerItems.begin() + i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return true; // Continue with strategy after knowing
                }
            }
        }

        // Priority 2: If we know it's a LIVE shell and shooting opponent
        if (dealerKnowledge.knowsCurrentShell && dealerKnowledge.currentShellIsLive) {
            // Use saw for double damage before shooting opponent
            for (int i = 0; i < dealerItems.size(); i++) {
                if (dealerItems[i].type == SAW && !sawActive) {
                    useItem(dealerItems[i], false);
                    dealerItems.erase(dealerItems.begin() + i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return true;
                }
            }
            return false; // Ready to shoot opponent
        }

        // Priority 3: If we know it's a BLANK shell
        if (dealerKnowledge.knowsCurrentShell && !dealerKnowledge.currentShellIsLive) {
            // Use handcuffs before shooting self with blank (get extra turn)
            for (int i = 0; i < dealerItems.size(); i++) {
                if (dealerItems[i].type == HANDCUFFS) {
                    useItem(dealerItems[i], false);
                    dealerItems.erase(dealerItems.begin() + i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return true;
                }
            }
            return false; // Ready to shoot self with blank
        }

        // Priority 4: If low HP, try to heal
        if (dealerHP <= maxHP / 2) {
            for (int i = 0; i < dealerItems.size(); i++) {
                if (dealerItems[i].type == CIGARETTE && dealerHP < maxHP) {
                    useItem(dealerItems[i], false);
                    dealerItems.erase(dealerItems.begin() + i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return true;
                }
            }
        }

        // Priority 5: If uncertain and have beer, eject shell (risky shell management)
        if (!dealerKnowledge.knowsCurrentShell && shells.size() - currentShell > 2) {
            for (int i = 0; i < dealerItems.size(); i++) {
                if (dealerItems[i].type == BEER) {
                    useItem(dealerItems[i], false);
                    dealerItems.erase(dealerItems.begin() + i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    dealerKnowledge.knowsCurrentShell = false; // Reset knowledge
                    return true;
                }
            }
        }

        // Priority 6: Use switch if we know it's live and want to shoot self
        if (dealerKnowledge.knowsCurrentShell && dealerKnowledge.currentShellIsLive) {
            for (int i = 0; i < dealerItems.size(); i++) {
                if (dealerItems[i].type == SWITCH) {
                    useItem(dealerItems[i], false);
                    dealerItems.erase(dealerItems.begin() + i);
                    dealerKnowledge.currentShellIsLive = false; // Now it's blank
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return true;
                }
            }
        }

        return false;
    }

    void useItem(const Item& item, bool isPlayer) {
        std::string user = isPlayer ? "You" : "Dealer";
        cout << user << " used " << item.name << endl;

        switch (item.type) {
        case MAGNIFYING_GLASS:
            if (currentShell < shells.size()) {
                std::string shellType = shells[currentShell] ? "LIVE" : "BLANK";
                if (isPlayer) {
                    cout << "Current shell: " << shellType << endl;
                }
                else {
                    cout << "Dealer examines the shell..." << endl;
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
            else if (!isPlayer && dealerHP < maxHP) {
                dealerHP++;
                cout << "Dealer healed 1 HP" << endl;
            }
            break;

        case HANDCUFFS:
            if (isPlayer) {
                dealerSkipTurn = true;
                //cout << "Dealer's turn will be skipped!" << endl;
            }
            else {
                // Dealer can't really handcuff player in current implementation
                // but you could add playerSkipTurn if you want
                cout << "Dealer used handcuffs!" << endl;
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
        std::string shooter = isPlayer ? "You" : "Dealer";
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
                dealerHP -= damage;
                setCursor(1, (211 - (maxHP * 14)));
                for (int i = 0; i < maxHP; i++) (i < (maxHP - dealerHP)) ? gun.out() : h1.out();
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

        std::string shooter = isPlayer ? "You" : "Dealer";
        std::string target = isPlayer ? "dealer" : "you";
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        bool isLive = shells[currentShell];
        currentShell++;

        if (isLive) {
            playSound("shotgun.wav", true);
            isPlayer ? shootp2.out(5, 93, 28, 14, 118) : shootp1.out(5, 93, 28, 14, 1);
			isPlayer ? shootp2.clearthis() : shootp1.clearthis();
            int damage = sawActive ? 2 : 1;
            if (isPlayer) {
                dealerHP -= damage;
                setCursor(1, 1);
                for (int i = 0; i < maxHP; i++) (i < playerHP) ? h1.out() : h1.clearthis();
            }
            else {
                playerHP -= damage;
                setCursor(1, (211 - (maxHP * 14)));
                for (int i = 0; i < maxHP; i++) (i < (maxHP - dealerHP)) ? gun.out() : h1.out();
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
        dealerHP = 3;
        maxHP = 3;
        maxItem = 6;
        currentShell = 0;
        playerTurn = true;
        dealerSkipTurn = false;
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
            while (playerHP > 0 && dealerHP > 0 && currentShell < shells.size()) {
                if (playerTurn) {
                    playerAction();
                }
                else {
                    if (dealerSkipTurn) {
                        clearScreen();
                        
						cout << "Dealer is handcuffed! Turn skipped." << endl;
                        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                        dealerSkipTurn = false;
                        playerTurn = true;
                    }
                    else {
                        dealerActionSmart();
                    }
                }
            }
            if (playerHP <= 0) {
                gameOver(false);
                break;
            }
            else if (dealerHP <= 0) {
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

int main() {
    enableANSI();
    cout << "\033]0;ANSI ROULETTE\007"; // Set console title
	setConsoleSize(211, 49);
	lockConsoleResize();
	//removeScrollbars();
    Game game;
    game.run();
    return 0;
}