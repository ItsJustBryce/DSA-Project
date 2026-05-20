#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using std::cout;
using std::cin;
using std::string;
using std::vector;
using std::getline;

// ============================================================================
// 1. DATA STRUCTURE
// ============================================================================
struct Flashcard {
    int id;
    string front;
    string back;
    string deck;
    string tag;
    string flag;
};

// Global database acting as our runtime system storage
vector<Flashcard> database;
int nextCardId = 1; // Tracks and assigns unique, ascending IDs to new cards

// ============================================================================
// 2. MANAGEMENT FUNCTIONS (Creating, Deleting, and Managing Decks/Cards)
// ============================================================================

// Instantiates a new card object and pushes it into our collection vector
void createFlashcard(string front, string back, string deck, string tag, string flag) {
    Flashcard newCard = { nextCardId++, front, back, deck, tag, flag };
    database.push_back(newCard);
    cout << "Success: Card ID " << newCard.id << " added to Deck '" << deck << "'.\n";
}

// Searches the vector for a matching ID and safely erases it from memory
void deleteFlashcard(int targetId) {
    for (auto it = database.begin(); it != database.end(); ++it) {
        if (it->id == targetId) {
            database.erase(it);
            cout << "Success: Card ID " << targetId << " permanently deleted from system storage.\n";
            return;
        }
    }
    cout << "Error: Card ID " << targetId << " not found in the database.\n";
}

// Relocates a card to a different deck simply by altering its deck string property
void updateCardDeck(int targetId, string newDeckName) {
    for (auto &card : database) {
        if (card.id == targetId) {
            card.deck = newDeckName;
            cout << "Success: Card ID " << targetId << " moved to Deck '" << newDeckName << "'.\n";
            return;
        }
    }
    cout << "Error: Card ID " << targetId << " not found.\n";
}

// ============================================================================
// 3. INTERACTIVE REVIEW SUBSYSTEM (Accessing and Answering)
// ============================================================================

// Loops through the database to prompt and test the user on cards matching a deck
void reviewDeck(string targetDeck) {
    cout << "\n=== Starting Review Session for Deck: " << targetDeck << " ===\n";
    int correctCount = 0;
    int totalReviewed = 0;
    cin.ignore(); // Clears trailing newlines from the input stream before taking strings

    for (const auto &card : database) {
        if (card.deck == targetDeck) {
            totalReviewed++;
            cout << "\n[Card " << totalReviewed << "] Front Prompt: " << card.front << "\n";
            
            string userResponse;
            cout << "Your Answer: ";
            getline(cin, userResponse);

            // Conditional exact string match confirmation
            if (userResponse == card.back) {
                cout << ">> CORRECT!\n";
                correctCount++;
            } else {
                cout << ">> INCORRECT.\n";
                cout << "   Expected Answer: " << card.back << "\n";
            }
        }
    }

    if (totalReviewed == 0) {
        cout << "No cards found to study inside deck: '" << targetDeck << "'.\n";
    } else {
        cout << "\n--- Session Summary ---\n";
        cout << "Final Score: " << correctCount << " / " << totalReviewed << " items answered correctly.\n";
    }
}

// ============================================================================
// 4. SEARCH & UTILITY FUNCTIONS (Santos's Core Requirements)
// ============================================================================

// Iterates through the collection and displays all fields matching a given deck
void searchByDeck(string targetDeck) {
    cout << "\n--- Search Results for Deck: " << targetDeck << " ---\n";
    bool found = false;
    for (const auto &card : database) {
        if (card.deck == targetDeck) {
            cout << "ID: " << card.id << " | Q: " << card.front << " | A: " << card.back << "\n";
            found = true;
        }
    }
    if (!found) cout << "No matching cards found inside that deck.\n";
}

// Filters card instances out of the database containing a specific string tag
void searchByTag(string targetTag) {
    cout << "\n--- Search Results for Tag: " << targetTag << " ---\n";
    bool found = false;
    for (const auto &card : database) {
        if (card.tag == targetTag) {
            cout << "ID: " << card.id << " | Q: " << card.front << " | A: " << card.back << "\n";
            found = true;
        }
    }
    if (!found) cout << "No cards matching that tag were located.\n";
}

// Filters card instances out of the database containing a specific colored flag
void searchByFlag(string targetFlag) {
    cout << "\n--- Search Results for Flag: " << targetFlag << " ---\n";
    bool found = false;
    for (const auto &card : database) {
        if (card.flag == targetFlag) {
            cout << "ID: " << card.id << " | Q: " << card.front << " | A: " << card.back << "\n";
            found = true;
        }
    }
    if (!found) cout << "No cards matching that flag color were located.\n";
}

// Compares all database elements using nested execution loops to find duplicate text strings
void findDuplicates() {
    cout << "\n--- Cross-Deck Duplicate Content Analysis ---\n";
    bool found = false;
    for (size_t i = 0; i < database.size(); i++) {
        for (size_t j = i + 1; j < database.size(); j++) {
            if (database[i].front == database[j].front) {
                cout << "Match Identified! \"" << database[i].front << "\" shared by IDs: " << database[i].id << " & " << database[j].id << "\n";
                found = true;
            }
        }
    }
    if (!found) cout << "Clean check. No duplicate card text sequences found.\n";
}

// Substitutes occurrences of exact substrings found inside any card question text block
void findAndReplace(string toFind, string toReplace) {
    cout << "\n--- Replacing exact strings: \"" << toFind << "\" -> \"" << toReplace << "\" ---\n";
    for (auto &card : database) { // Changes are applied directly by referencing memory via '&'
        size_t pos = 0;
        while ((pos = card.front.find(toFind, pos)) != string::npos) {
            card.front.replace(pos, toFind.length(), toReplace);
            pos += toReplace.length(); // Slides search track target past the alteration boundary
        }
    }
    cout << "Global find-and-replace sweep completed.\n";
}

// ============================================================================
// 5. MAIN EXECUTIVE LOOP
// ============================================================================
int main() {
    // Immediate out-of-the-box seed data injection for your demonstration
    createFlashcard("What is an Array?", "Linear collection", "CS", "basics", "Red");
    createFlashcard("What is a Stack?", "LIFO structure", "CS", "advanced", "Blue");
    createFlashcard("What is an Array?", "Identical block items", "Data Structures", "basics", "None");

    int choice = 0;
    while (choice != 9) {
        cout << "\n=========================================\n";
        cout << "   ANKI PLATFORM INTERACTIVE CONSOLE     \n";
        cout << "=========================================\n";
        cout << "1. Create Flashcard (Add to Deck)\n";
        cout << "2. Delete Flashcard\n";
        cout << "3. Move Card to Another Deck\n";
        cout << "4. STUDY / ANSWER DECK\n";
        cout << "5. Search by Deck Name\n";
        cout << "6. Search by Tag\n";
        cout << "7. Find Duplicates Across System\n";
        cout << "8. Find and Replace Content Text\n";
        cout << "9. Exit Simulation\n";
        cout << "Selection -> ";
        cin >> choice;

        if (cin.fail()) { // Sanitizes bad numeric data inputs gracefully
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1) {
            string q, a, d, t, f;
            cin.ignore();
            cout << "Question: "; getline(cin, q);
            cout << "Answer: ";   getline(cin, a);
            cout << "Deck: ";     getline(cin, d);
            cout << "Tag: ";      getline(cin, t);
            cout << "Flag: ";     getline(cin, f);
            createFlashcard(q, a, d, t, f);
        }
        else if (choice == 2) {
            int id; cout << "Enter Card ID to destroy: "; cin >> id;
            deleteFlashcard(id);
        }
        else if (choice == 3) {
            int id; string newDeck;
            cout << "Enter Card ID: "; cin >> id;
            cout << "Enter target Deck destination name: "; cin.ignore(); getline(cin, newDeck);
            updateCardDeck(id, newDeck);
        }
        else if (choice == 4) {
            string deckToStudy;
            cout << "Enter the name of the deck you want to study: "; cin.ignore();
            getline(cin, deckToStudy);
            reviewDeck(deckToStudy);
        }
        else if (choice == 5) { string d; cout << "Deck name: "; cin.ignore(); getline(cin, d); searchByDeck(d); }
        else if (choice == 6) { string t; cout << "Tag name: ";  cin.ignore(); getline(cin, t); searchByTag(t); }
        else if (choice == 7) { findDuplicates(); }
        else if (choice == 8) {
            string f, r;
            cin.ignore();
            cout << "Find exact phrase: "; getline(cin, f);
            cout << "Replace with: ";      getline(cin, r);
            findAndReplace(f, r);
        }
    }
    return 0;
}