#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

using namespace std;

// ============================================================================
// 1. DATA STRUCTURES & BLUEPRINTS
// ============================================================================

// [LOBOS / BERNARDINO] - Card Types representation
enum class CardType { Basic, Reversed, TypeInAnswer };

// [RAMOS / LOBOS / BERNARDINO] - Core Flashcard Structure
struct Flashcard {
    int id;
    string front;
    string back;
    CardType type;       // [LOBOS / BERNARDINO]
    vector<string> tags; // [LOBOS]
    string flag = "None";// [LOBOS]
    
    // [MARQUEZ / PONCE] - Spaced Repetition (FSRS) parameters
    double stability = 2.0;
    double difficulty = 5.0;
    int intervals = 1;
};

// [RAMOS / BERNARDINO] - Hierarchical Deck Structure
struct Deck {
    string name;
    vector<Flashcard> cards;
    vector<shared_ptr<Deck>> subDecks; // [BERNARDINO] - Hierarchical Nesting
};

// [TOMINES] - User Profile Structure
struct Profile {
    string name;
    vector<shared_ptr<Deck>> rootDecks;
    
    // [MARQUEZ / PONCE] - Statistics Tracking
    int totalReviews = 0;
    int correctReviews = 0;
};

// [TOMINES] - Main Account Structure
struct Account {
    string username;
    string password;
    vector<Profile> profiles;
    shared_ptr<Profile> activeProfile = nullptr;
};

// Global Data States
vector<Account> accounts;
Account* activeAccount = nullptr;
vector<vector<Account>> systemBackups; // [MARQUEZ / PONCE] - Global Backup Matrix

// ============================================================================
// HIERARCHICAL LOOKUP HELPERS
// ============================================================================
shared_ptr<Deck> findDeckRecursive(shared_ptr<Deck> current, const string& deckName) {
    if (current->name == deckName) return current;
    for (auto sub : current->subDecks) {
        auto found = findDeckRecursive(sub, deckName);
        if (found) return found;
    }
    return nullptr;
}

shared_ptr<Deck> getDeck(const string& deckName) {
    if (!activeAccount || !activeAccount->activeProfile) return nullptr;
    for (auto root : activeAccount->activeProfile->rootDecks) {
        auto found = findDeckRecursive(root, deckName);
        if (found) return found;
    }
    return nullptr;
}

void gatherCardsRecursive(shared_ptr<Deck> current, vector<Flashcard*>& cardRefs) {
    for (auto& card : current->cards) {
        cardRefs.push_back(&card);
    }
    for (auto sub : current->subDecks) {
        gatherCardsRecursive(sub, cardRefs);
    }
}

Flashcard* findCardById(int id) {
    if (!activeAccount || !activeAccount->activeProfile) return nullptr;
    for (auto root : activeAccount->activeProfile->rootDecks) {
        vector<Flashcard*> refs;
        gatherCardsRecursive(root, refs);
        for (auto ref : refs) {
            if (ref->id == id) return ref;
        }
    }
    return nullptr;
}

int generateGlobalCardId() {
    static int idCounter = 100;
    return idCounter++;
}

// ============================================================================
// 2. MEMBER DELEGATED MODULES
// ============================================================================

// ----------------------------------------------------------------------------
// MEMBER: TOMINES (Account & Profile Management Tasks)
// ----------------------------------------------------------------------------
void createAccount(string user, string pass) {
    accounts.push_back({user, pass, {}, nullptr});
    cout << "Account created successfully.\n";
}

void switchAccount(string user, string pass) {
    for (auto& acc : accounts) {
        if (acc.username == user && acc.password == pass) {
            activeAccount = &acc;
            cout << "Switched to account: " << user << "\n";
            return;
        }
    }
    cout << "Invalid account credentials.\n";
}

void updateAccount(string newPass) {
    if (activeAccount) {
        activeAccount->password = newPass;
        cout << "Account password updated.\n";
    }
}

void deleteAccount() {
    if (!activeAccount) return;
    for (auto it = accounts.begin(); it != accounts.end(); ++it) {
        if (it->username == activeAccount->username) {
            accounts.erase(it);
            activeAccount = nullptr;
            cout << "Account deleted successfully.\n";
            return;
        }
    }
}

void createProfile(string name) {
    if (!activeAccount) return;
    activeAccount->profiles.push_back({name, {}, 0, 0});
    if (!activeAccount->activeProfile) {
        activeAccount->activeProfile = make_shared<Profile>(activeAccount->profiles.back());
    }
    cout << "Profile '" << name << "' created.\n";
}

void switchProfile(string name) {
    if (!activeAccount) return;
    for (auto& prof : activeAccount->profiles) {
        if (prof.name == name) {
            activeAccount->activeProfile = make_shared<Profile>(prof);
            cout << "Switched to profile: " << name << "\n";
            return;
        }
    }
    cout << "Profile not found.\n";
}

void updateProfile(string newName) {
    if (activeAccount && activeAccount->activeProfile) {
        activeAccount->activeProfile->name = newName;
        cout << "Profile name updated.\n";
    }
}

void deleteProfile() {
    if (!activeAccount || !activeAccount->activeProfile) return;
    for (auto it = activeAccount->profiles.begin(); it != activeAccount->profiles.end(); ++it) {
        if (it->name == activeAccount->activeProfile->name) {
            activeAccount->profiles.erase(it);
            activeAccount->activeProfile = nullptr;
            cout << "Profile deleted successfully.\n";
            return;
        }
    }
}

// ----------------------------------------------------------------------------
// MEMBER: RAMOS (Core Deck & Card Controls)
// ----------------------------------------------------------------------------
void updateDeck(string oldName, string newName) {
    auto d = getDeck(oldName);
    if (d) {
        d->name = newName;
        cout << "Deck name updated successfully.\n";
    }
}

void deleteDeck(string name) {
    if (!activeAccount || !activeAccount->activeProfile) return;
    auto& roots = activeAccount->activeProfile->rootDecks;
    for (auto it = roots.begin(); it != roots.end(); ++it) {
        if ((*it)->name == name) { roots.erase(it); cout << "Deck deleted.\n"; return; }
    }
    cout << "Deck not found at root level.\n";
}

void updateCard(int id, string newFront, string newBack) {
    auto card = findCardById(id);
    if (card) {
        card->front = newFront;
        card->back = newBack;
        cout << "Card updated successfully.\n";
    }
}

void deleteCard(int id) {
    if (!activeAccount || !activeAccount->activeProfile) return;
    for (auto root : activeAccount->activeProfile->rootDecks) {
        auto d = root;
        for (auto it = d->cards.begin(); it != d->cards.end(); ++it) {
            if (it->id == id) { d->cards.erase(it); cout << "Card deleted.\n"; return; }
        }
    }
}

// ----------------------------------------------------------------------------
// MEMBERS: BERNARDINO & LOBOS (Nesting, Complex Hierarchies, & Advanced Card Types)
// ----------------------------------------------------------------------------

// [BERNARDINO] - Nesting Decks & Multiple Hierarchies
void createDeck(string name, string parentName = "") {
    if (!activeAccount || !activeAccount->activeProfile) return;
    auto newDeck = make_shared<Deck>(Deck{name, {}, {}});
    
    if (parentName.empty()) {
        activeAccount->activeProfile->rootDecks.push_back(newDeck);
        cout << "Root Deck '" << name << "' created.\n";
    } else {
        auto parent = getDeck(parentName);
        if (parent) {
            parent->subDecks.push_back(newDeck); 
            cout << "Nested Deck '" << name << "' added under '" << parentName << "'.\n";
        } else {
            cout << "Parent deck not found.\n";
        }
    }
}

// [LOBOS / BERNARDINO] - Advanced Card Creation & Reversed/Typed Input Automation
void createCard(string deckName, string f, string b, int typeOption) {
    auto targetDeck = getDeck(deckName);
    if (!targetDeck) { cout << "Target deck configuration missing.\n"; return; }
    
    CardType cType = CardType::Basic;
    if (typeOption == 2) cType = CardType::Reversed;
    if (typeOption == 3) cType = CardType::TypeInAnswer;

    int newId = generateGlobalCardId();
    targetDeck->cards.push_back({newId, f, b, cType, {}, "None"});
    cout << "Card added with ID: " << newId << "\n";

    if (cType == CardType::Reversed) {
        targetDeck->cards.push_back({generateGlobalCardId(), b, f, CardType::Basic, {}, "None"});
        cout << "-> Automated reversed card variation generated successfully.\n";
    }
}

// [BERNARDINO / RAMOS] - Selective hierarchy parsing reviews 
void reviewDeckSelective(string deckName) {
    auto targetDeck = getDeck(deckName);
    if (!targetDeck) return;

    vector<Flashcard*> reviewPool;
    gatherCardsRecursive(targetDeck, reviewPool); 

    cout << "\n--- Interactive Review Session ---\n";
    cin.ignore();
    for (auto card : reviewPool) {
        cout << "\nQuestion: " << card->front << "\n";
        if (card->type == CardType::TypeInAnswer) cout << "[Requires User-Typed Input Verification Match]\n";
        
        string ans;
        cout << "Your Answer: ";
        getline(cin, ans);

        // [MARQUEZ / PONCE] - FSRS Evaluation Metrics Hooks inside Reviews
        bool isCorrect = (ans == card->back);
        activeAccount->activeProfile->totalReviews++;
        
        if (isCorrect) {
            cout << "Correct!\n";
            activeAccount->activeProfile->correctReviews++;
            card->stability *= 1.5; 
        } else {
            cout << "Incorrect! Answer was: " << card->back << "\n";
            card->stability *= 0.5;
        }
    }
}

// [LOBOS] - Tags and Flags management operations 
void manageTags(int cardId, int op, string tagName) {
    auto c = findCardById(cardId);
    if (!c) return;
    if (op == 1) c->tags.push_back(tagName); 
    if (op == 2) c->tags.erase(remove(c->tags.begin(), c->tags.end(), tagName), c->tags.end()); 
    cout << "Tags system array updated.\n";
}

void manageFlags(int cardId, string flagColor) {
    auto c = findCardById(cardId);
    if (c) { c->flag = flagColor; cout << "Flag updated successfully.\n"; }
}

// ----------------------------------------------------------------------------
// MEMBER: SANTOS (Your Core Utilities: Search, Duplicates, Exact Replace)
// ----------------------------------------------------------------------------
void searchByDeck(string name) {
    auto d = getDeck(name);
    if (!d) return;
    cout << "\nListing cards for deck [" << name << "]:\n";
    for (auto c : d->cards) cout << "ID: " << c.id << " | Front: " << c.front << "\n";
}

void searchByTag(string tag) {
    if (!activeAccount || !activeAccount->activeProfile) return;
    cout << "\nFiltering items by exact tag balance match -> " << tag << ":\n";
    for (auto root : activeAccount->activeProfile->rootDecks) {
        vector<Flashcard*> refs; gatherCardsRecursive(root, refs);
        for (auto c : refs) {
            if (find(c->tags.begin(), c->tags.end(), tag) != c->tags.end())
                cout << "ID: " << c->id << " | Front: " << c->front << "\n";
        }
    }
}

void searchByFlag(string flag) {
    if (!activeAccount || !activeAccount->activeProfile) return;
    cout << "\nFiltering items by flag classification -> " << flag << ":\n";
    for (auto root : activeAccount->activeProfile->rootDecks) {
        vector<Flashcard*> refs; gatherCardsRecursive(root, refs);
        for (auto c : refs) {
            if (c->flag == flag) cout << "ID: " << c->id << " | Front: " << c->front << "\n";
        }
    }
}

void findDuplicates() {
    if (!activeAccount || !activeAccount->activeProfile) return;
    cout << "\nChecking for matching duplicate textual question chains:\n";
    vector<Flashcard*> refs;
    for (auto root : activeAccount->activeProfile->rootDecks) gatherCardsRecursive(root, refs);
    
    for (size_t i = 0; i < refs.size(); i++) {
        for (size_t j = i + 1; j < refs.size(); j++) {
            if (refs[i]->front == refs[j]->front) {
                cout << "Match Found! Description: \"" << refs[i]->front << "\" shared by IDs " << refs[i]->id << " and " << refs[j]->id << "\n";
            }
        }
    }
}

void findAndReplace(string toFind, string toReplace) {
    if (!activeAccount || !activeAccount->activeProfile) return;
    vector<Flashcard*> refs;
    for (auto root : activeAccount->activeProfile->rootDecks) gatherCardsRecursive(root, refs);
    for (auto c : refs) {
        size_t pos = 0;
        while ((pos = c->front.find(toFind, pos)) != string::npos) {
            c->front.replace(pos, toFind.length(), toReplace);
            pos += toReplace.length();
        }
    }
    cout << "Exact global text array replacements completed.\n";
}

// ----------------------------------------------------------------------------
// MEMBERS: MARQUEZ & PONCE (Advanced Extra System Settings, FSRS, & Data Backups)
// ----------------------------------------------------------------------------
void displayStatistics() {
    if (!activeAccount || !activeAccount->activeProfile) return;
    auto p = activeAccount->activeProfile;
    cout << "\n=== Profile Run Performance Statistics ===\n";
    cout << "Session Tracker Module: " << p->name << "\n";
    cout << "Processed Study Repetitions: " << p->totalReviews << "\n";
    cout << "Accurate Matches Evaluated: " << p->correctReviews << "\n";
}

void createBackup() {
    systemBackups.push_back(accounts);
    cout << "System fallback matrix checkpoint generated.\n";
}

void revertToBackup() {
    if (!systemBackups.empty()) {
        accounts = systemBackups.back();
        activeAccount = nullptr;
        cout << "System environment restored to historical checkpoint tracking reference state.\n";
    } else {
        cout << "No existing backups found.\n";
    }
}

// ============================================================================
// 3. MAIN RUNTIME LOOP TERMINAL INTERFACE
// ============================================================================
int main() {
    createAccount("admin", "123");
    switchAccount("admin", "123");
    createProfile("Default");
    createDeck("Programming");
    createDeck("C++", "Programming"); 
    createCard("C++", "What is a Pointer?", "An address manager", 1);
    createCard("C++", "What is a Reference?", "An alias string", 3);

    int choice = 0;
    while (choice != 11) {
        cout << "\n==================================================\n";
        cout << "   ANKI PLATFORM SYSTEM TERMINAL HIERARCHY        \n";
        cout << "==================================================\n";
        cout << "1. [TOMINES] Accounts Control Module\n";
        cout << "2. [TOMINES] Profiles Control Module\n";
        cout << "3. [RAMOS/BERNARDINO] Decks & Nesting Management\n";
        cout << "4. [RAMOS/LOBOS/BERNARDINO] Cards Management Suite\n";
        cout << "5. [RAMOS/BERNARDINO/MARQUEZ/PONCE] Run Selected Review Sessions\n";
        cout << "6. [SANTOS] Global Database Search Engines\n";
        cout << "7. [SANTOS] Scan Core Duplicate Strings\n";
        cout << "8. [SANTOS] Global Text Find and Replace Utility\n";
        cout << "9. [MARQUEZ/PONCE] Display Performance Statistics\n";
        cout << "10. [MARQUEZ/PONCE] System Snapshots and Backups\n";
        cout << "11. Exit Environment\n";
        cout << "Select Operation Option -> ";
        cin >> choice;

        if (choice == 1) {
            int subOpt; cout << "1. Create Acc 2. Switch Acc 3. Change Pass 4. Delete Acc: "; cin >> subOpt;
            string u, p; if (subOpt == 1 || subOpt == 2) { cout << "User: "; cin >> u; cout << "Pass: "; cin >> p; }
            if (subOpt == 1) createAccount(u, p);
            if (subOpt == 2) switchAccount(u, p);
            if (subOpt == 3) { string np; cout << "New Pass: "; cin >> np; updateAccount(np); }
            if (subOpt == 4) deleteAccount();
        }
        else if (choice == 2) {
            int subOpt; cout << "1. Create Profile 2. Switch Profile 3. Delete Profile: "; cin >> subOpt;
            string n; cout << "Profile Name: "; cin >> n;
            if (subOpt == 1) createProfile(n);
            if (subOpt == 2) switchProfile(n);
            if (subOpt == 3) deleteProfile();
        }
        else if (choice == 3) {
            int subOpt; cout << "1. Create Root Deck 2. Create Nested Subdeck 3. Delete Deck: "; cin >> subOpt;
            string n, p = ""; cout << "Deck Name: "; cin >> n;
            if (subOpt == 2) { cout << "Parent Deck Name: "; cin >> p; }
            if (subOpt == 1 || subOpt == 2) createDeck(n, p);
            if (subOpt == 3) deleteDeck(n);
        }
        else if (choice == 4) {
            int subOpt; cout << "1. Add Card 2. Delete Card 3. Update Tag 4. Set Flag Color: "; cin >> subOpt;
            if (subOpt == 1) {
                string d, f, b; int t;
                cout << "Deck Name: "; cin >> d; cout << "Front: "; cin.ignore(); getline(cin, f);
                cout << "Back: "; getline(cin, b); cout << "Type (1:Basic, 2:Reversed, 3:TypeInAnswer): "; cin >> t;
                createCard(d, f, b, t);
            } else if (subOpt == 2) {
                int id; cout << "Card ID: "; cin >> id; deleteCard(id);
            } else if (subOpt == 3) {
                int id, tOpt; string tName; cout << "Card ID: "; cin >> id; cout << "1. Add 2. Remove: "; cin >> tOpt;
                cout << "Tag Label: "; cin >> tName; manageTags(id, tOpt, tName);
            } else if (subOpt == 4) {
                int id; string fl; cout << "Card ID: "; cin >> id; cout << "Flag Color: "; cin >> fl;
                manageFlags(id, fl);
            }
        }
        else if (choice == 5) {
            string d; cout << "Enter exact target deck string: "; cin >> d;
            reviewDeckSelective(d);
        }
        else if (choice == 6) {
            int subOpt; cout << "1. By Deck 2. By Tag 3. By Flag: "; cin >> subOpt;
            string q; cout << "Query term: "; cin >> q;
            if (subOpt == 1) searchByDeck(q);
            if (subOpt == 2) searchByTag(q);
            if (subOpt == 3) searchByFlag(q);
        }
        else if (choice == 7) { findDuplicates(); }
        else if (choice == 8) {
            string f, r; cout << "Find text: "; cin.ignore(); getline(cin, f);
            cout << "Replace with: "; getline(cin, r);
            findAndReplace(f, r);
        }
        else if (choice == 9) { displayStatistics(); }
        else if (choice == 10) {
            int subOpt; cout << "1. Save System State Backup 2. Revert to Last Backup Point: "; cin >> subOpt;
            if (subOpt == 1) createBackup();
            if (subOpt == 2) revertToBackup();
        }
    }
    return 0;
}