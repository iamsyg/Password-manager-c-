#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>

class Credential {
public:
    std::string username;
    std::string password;
    std::string website;

    Credential() = default;
    Credential(std::string uname, std::string passwd, std::string site)
        : username(std::move(uname)), password(std::move(passwd)), website(std::move(site)) {}
};

class PasswordManager {
private:
    std::vector<Credential> credentials;
    const std::string filename;
    const char encryptionKey = 'K'; // Simple key for XOR encryption

    // Encryption function
    std::string encryptDecrypt(const std::string& input) const {
        std::string result = input;
        for (char& c : result) {
            c ^= encryptionKey;
        }
        return result;
    }

    void heapify(std::vector<Credential>& arr, int n, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < n && arr[left].website > arr[largest].website)
            largest = left;

        if (right < n && arr[right].website > arr[largest].website)
            largest = right;

        if (largest != i) {
            std::swap(arr[i], arr[largest]);
            heapify(arr, n, largest);
        }
    }

    void heapSort(std::vector<Credential>& arr) {
        int n = arr.size();
        for (int i = n / 2 - 1; i >= 0; i--)
            heapify(arr, n, i);

        for (int i = n - 1; i > 0; i--) {
            std::swap(arr[0], arr[i]);
            heapify(arr, i, 0);
        }
    }

    void saveToFile() {
        std::ofstream file(filename, std::ios::binary);
        if (file.is_open()) {
            for (auto& cred : credentials) {
                file << cred.username << "\n"
                     << encryptDecrypt(cred.password) << "\n" // Save encrypted password
                     << cred.website << "\n";
            }
            file.close();
        } else {
            std::cerr << "Error saving data to file.\n";
        }
    }

    void loadFromFile() {

        // credentials.clear();
        // std::ifstream file(filename);
        // if (file.is_open()) {
        //     std::string uname, passwd, site;
        //     while (getline(file, uname) && getline(file, passwd) && getline(file, site)) {
        //         credentials.emplace_back(uname, encryptDecrypt(passwd), site); // Decrypt password on load
        //     }
        //     file.close();
        //     heapSort(credentials); // Sort after loading
        // } else {
        //     std::cerr << "Error reading data from file.\n";
        // }

        
        credentials.clear();
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string uname, passwd, site;
            while (getline(file, uname) && getline(file, passwd) && getline(file, site)) {
                credentials.emplace_back(uname, encryptDecrypt(passwd), site); // Decrypt password on load
            }
            file.close();
            heapSort(credentials); // Sort after loading
        } else {
            std::cerr << "Error reading data from file.\n";
        }
    }

    int binarySearch(const std::string& website) const {
        int low = 0, high = credentials.size() - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (credentials[mid].website == website) {
                return mid;
            } else if (credentials[mid].website < website) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return -1; // Not found
    }

public:
    explicit PasswordManager(std::string file) : filename(std::move(file)) {
        loadFromFile();
    }

    void addCredential(const std::string& username, const std::string& password, const std::string& website) {
        int index = binarySearch(website);
        if (index != -1) {
            std::cout << "Credential for the website '" << website << "' already exists.\n";
            return;
        }

        // Add the new credential
        credentials.emplace_back(username, password, website);
        heapSort(credentials); // Sort credentials after adding
        saveToFile();
        std::cout << "Credential added successfully!\n";
    }

    void retrieveCredentials(const std::string& website) {
        int index = binarySearch(website);
        if (index != -1) {
            const auto& cred = credentials[index];
            std::cout << "Website: " << cred.website << "\n"
                      << "Username: " << cred.username << "\n"
                      << "Password: " << cred.password << "\n\n";
        } else {
            std::cout << "No credentials found for the website: " << website << "\n";
        }
    }

    void deleteCredential(const std::string& website) {
        int index = binarySearch(website);
        if (index != -1) {
            credentials.erase(credentials.begin() + index);
            saveToFile();
            std::cout << "Credential deleted successfully!\n";
        } else {
            std::cout << "No credentials found for the website: " << website << "\n";
        }
    }

    void updateCredential(const std::string& website) {

        // int index = binarySearch(website);

    // if (index == -1) {
    //     std::cout << "No credentials found for the website: " << website << "\n";
    //     return;
    // }

    // // Display existing credentials
    // std::cout << "Existing credentials:\n";
    // std::cout << "Website: " << credentials[index].website << "\n"
    //           << "Username: " << credentials[index].username << "\n"
    //           << "Password: " << encryptDecrypt(credentials[index].password) << "\n";

    // // Get new credentials from the user
    // std::string newUsername, newPassword;
    // std::cout << "Enter new username: ";
    // std::getline(std::cin, newUsername);
    // std::cout << "Enter new password: ";
    // std::getline(std::cin, newPassword);

    // // Update the credentials
    // credentials[index].username = newUsername;
    // credentials[index].password = encryptDecrypt(newPassword);

    // // Save updated credentials to the file
    // saveToFile();
    // std::cout << "Credentials updated successfully!\n";


        int index = binarySearch(website);

        if (index == -1) {
            std::cout << "No credentials found for the website: " << website << "\n";
            return;
        }

        const auto& existing = credentials[index];
        std::cout << "Existing credentials:\n";
        std::cout << "Website: " << existing.website << "\n"
                  << "Username: " << existing.username << "\n"
                  << "Password: " << existing.password << "\n";

        std::string newUsername, newPassword;
        std::cout << "Enter new username: ";
        std::getline(std::cin, newUsername);
        std::cout << "Enter new password: ";
        std::getline(std::cin, newPassword);

        credentials[index].username = newUsername;
        credentials[index].password = newPassword;

        heapSort(credentials);
        saveToFile();
        std::cout << "Credentials updated successfully!\n";
    }

    void listAllCredentials() {

        // if (credentials.empty()) {
        //     std::cout << "No credentials stored.\n";
        //     return;
        // }

        // std::cout << std::left << std::setw(20) << "Website"
        //           << std::setw(20) << "Username"
        //           << std::setw(20) << "Password" << "\n";
        // std::cout << std::string(60, '-') << "\n";

        // for (const auto& cred : credentials) {
        //     std::cout << std::left << std::setw(20) << cred.website
        //               << std::setw(20) << cred.username
        //               << std::setw(20) << cred.password << "\n";
        // }
        // std::cout << "\n";


        if (credentials.empty()) {
            std::cout << "No credentials stored.\n";
            return;
        }

        std::cout << std::left << std::setw(20) << "Website"
                  << std::setw(20) << "Username"
                  << std::setw(20) << "Password" << "\n";
        std::cout << std::string(60, '-') << "\n";

        for (const auto& cred : credentials) {
            std::cout << std::left << std::setw(20) << cred.website
                      << std::setw(20) << cred.username
                      << std::setw(20) << cred.password << "\n";
        }
        std::cout << "\n";
    }
};

int main() {
    const std::string MASTER_PASSWORD = "admin123"; // Master password
    std::string enteredPassword;

    std::cout << "Enter master password to access the Password Manager: ";
    std::getline(std::cin, enteredPassword);

    if (enteredPassword != MASTER_PASSWORD) {
        std::cout << "Incorrect master password. Access denied.\n";
        return 1;
    }

    PasswordManager manager("credentials.txt");
    int choice;
    std::string username, password, website;

    do {
        std::cout << "\nPassword Manager\n"
                  << "1. Add Credential\n"
                  << "2. Retrieve Credential\n"
                  << "3. Delete Credential\n"
                  << "4. Update Credential\n"
                  << "5. List All Credentials\n"
                  << "6. Exit\n"
                  << "Enter your choice: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number between 1 and 6.\n";
            continue;
        }

        std::cin.ignore();
        switch (choice) {
            case 1:
                std::cout << "Enter website: ";
                getline(std::cin, website);
                std::cout << "Enter username: ";
                getline(std::cin, username);
                std::cout << "Enter password: ";
                getline(std::cin, password);
                manager.addCredential(username, password, website);
                break;
            case 2:
                std::cout << "Enter website to retrieve credentials: ";
                getline(std::cin, website);
                manager.retrieveCredentials(website);
                break;
            case 3:
                std::cout << "Enter website to delete credentials: ";
                getline(std::cin, website);
                manager.deleteCredential(website);
                break;
            case 4:
                std::cout << "Enter website to update credentials: ";
                std::getline(std::cin, website);
                manager.updateCredential(website);
                break;
            case 5:
                manager.listAllCredentials();
                break;
            case 6:
                std::cout << "Exiting Password Manager.\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 6);

    return 0;
}
