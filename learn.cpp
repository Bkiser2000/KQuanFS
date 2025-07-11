#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <mutex>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <set>

// Logic Gates
bool AND(bool a, bool b) { return a && b; }
bool OR(bool a, bool b) { return a || b; } 
bool NOT(bool a) { return !a; } 

// Metadata and Quantum File Structure
struct FileMetadata {
    std::string owner;
    std::string permissions;
    std::string timestamp;
    size_t size;
};

//Qbit structure
struct Qbit {
    double alpha; // amplitude for |0>
    double beta;  // amplitude for |1>
    bool measured = false;
    int value = -1; // -1 not measured, 0 or 1: measured value
};
struct QuantumFile {
    std::string name;
    std::vector<std::string> possible_contents; // Superposition
    std::vector<FileMetadata> possible_metadata; // Superposition of metadata
    std::vector<Qbit> qbits; 
    std::string observed_content;
    FileMetadata observed_metadata;
};

struct QuantumDirectory {
    std::string name;
    std::vector<QuantumFile> files;
    std::vector<QuantumDirectory> subdirs;
};

// Logic Gate Parameterization
enum class LogicGate { AND, OR, NOT };

bool apply_gate(LogicGate gate, bool a, bool b = false) {
    switch (gate) {
        case LogicGate::AND: return a && b;
        case LogicGate::OR:  return a || b;
        case LogicGate::NOT: return !a;
    }
    return false;
}

LogicGate get_user_gate() {
    std::cout << "Choose logic gate for evolution (and/or/not): ";
    std::string gate;
    std::cin >> gate;
    if (gate == "and") return LogicGate::AND;
    if (gate == "or")  return LogicGate::OR;
    if (gate == "not") return LogicGate::NOT;
    return LogicGate::AND; // default
}

// Entanglement structure
struct Entanglement {
    QuantumFile* fileA;
    QuantumFile* fileB;
    bool active;
    std::string integrity_hash;
};

std::string compute_integrity(const QuantumFile& qf) {
    std::hash<std::string> hasher;
    size_t h = 0;
    for (const auto& c : qf.possible_contents) h ^= hasher(c);
    return std::to_string(h);
}

void entangle(Entanglement& ent, QuantumFile& a, QuantumFile& b) {
    ent.fileA = &a;
    ent.fileB = &b;
    ent.active = true;
    ent.integrity_hash = compute_integrity(a) + compute_integrity(b);
}

void disentangle(Entanglement& ent) {
    ent.active = false;
}

bool check_entanglement_integrity(const Entanglement& ent) {
    if (!ent.active) return true;
    std::string current = compute_integrity(*ent.fileA) + compute_integrity(*ent.fileB);
    return current == ent.integrity_hash;
}

// Parallel Evolution using logic gates
void evolve_state(QuantumFile& qf, std::mutex& mtx, int branch_id, LogicGate gate, bool cond1, bool cond2) {
    if (apply_gate(gate, cond1, cond2)) {
        std::lock_guard<std::mutex> lock(mtx);
        qf.possible_contents.push_back("Branch_" + std::to_string(branch_id) + "_State_" + std::to_string(rand() % 100));
        FileMetadata meta = {"user" + std::to_string(branch_id), "rw-r--r--", "2025-06-28", static_cast<size_t>(rand() % 1000)};
        qf.possible_metadata.push_back(meta);
    }
}

// Measurement (Collapse)
void measure(QuantumFile& qf) {
    if (!qf.possible_contents.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, qf.possible_contents.size() - 1);
        int idx = dis(gen);
        qf.observed_content = qf.possible_contents[idx];
        if (!qf.possible_metadata.empty())
            qf.observed_metadata = qf.possible_metadata[idx % qf.possible_metadata.size()];
    }
}

// Serialization
void serialize(const QuantumFile& qf, const std::string& path) {
    std::ofstream ofs(path, std::ios::binary);
    size_t n = qf.possible_contents.size();
    ofs.write(reinterpret_cast<const char*>(&n), sizeof(n));
    for (const auto& content : qf.possible_contents) {
        size_t len = content.size();
        ofs.write(reinterpret_cast<const char*>(&len), sizeof(len));
        ofs.write(content.c_str(), len);
    }
    size_t m = qf.possible_metadata.size();
    ofs.write(reinterpret_cast<const char*>(&m), sizeof(m));
    for (const auto& meta : qf.possible_metadata) {
        size_t len_owner = meta.owner.size();
        ofs.write(reinterpret_cast<const char*>(&len_owner), sizeof(len_owner));
        ofs.write(meta.owner.c_str(), len_owner);
        size_t len_perm = meta.permissions.size();
        ofs.write(reinterpret_cast<const char*>(&len_perm), sizeof(len_perm));
        ofs.write(meta.permissions.c_str(), len_perm);
        size_t len_time = meta.timestamp.size();
        ofs.write(reinterpret_cast<const char*>(&len_time), sizeof(len_time));
        ofs.write(meta.timestamp.c_str(), len_time);
        ofs.write(reinterpret_cast<const char*>(&meta.size), sizeof(meta.size));
    }
        size_t q = qf.qbits.size();
        ofs.write(reinterpret_cast<const char*>(&q), sizeof(q));
        for (const auto& qb : qf.qbits) {
            ofs.write(reinterpret_cast<const char*>(&qb.alpha), sizeof(qb.alpha));
            ofs.write(reinterpret_cast<const char*>(&qb.beta), sizeof(qb.beta));
            ofs.write(reinterpret_cast<const char*>(&qb.measured), sizeof(qb.measured));
            ofs.write(reinterpret_cast<const char*>(&qb.value), sizeof(qb.value));
    }
    ofs.close();

}
// Deserialization
void deserialize(QuantumFile& qf, const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) {
        std::cerr << "Failed to open file for reading: " << path << std::endl;
        return;
    }
    size_t n;
    ifs.read(reinterpret_cast<char*>(&n), sizeof(n));
    if (n > 10000) { // Arbitrary sanity limit
        std::cerr << "Unreasonable number of contents: " << n << std::endl;
        return;
    }
    qf.possible_contents.resize(n);
    for (size_t i = 0; i < n; ++i) {
        size_t len;
        ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
        if (len > 1000000) { // Arbitrary sanity limit
            std::cerr << "Unreasonable content length: " << len << std::endl;
            return;
        }
        qf.possible_contents[i].resize(len);
        ifs.read(&qf.possible_contents[i][0], len);
    }
    size_t m;
    ifs.read(reinterpret_cast<char*>(&m), sizeof(m));
    if (m > 10000) {
        std::cerr << "Unreasonable number of metadata entries: " << m << std::endl;
        return;
    }
    qf.possible_metadata.resize(m);
    for (size_t i = 0; i < m; ++i) {
        size_t len_owner, len_perm, len_time;
        ifs.read(reinterpret_cast<char*>(&len_owner), sizeof(len_owner));
        if (len_owner > 1000) return;
        qf.possible_metadata[i].owner.resize(len_owner);
        ifs.read(&qf.possible_metadata[i].owner[0], len_owner);
        ifs.read(reinterpret_cast<char*>(&len_perm), sizeof(len_perm));
        if (len_perm > 1000) return;
        qf.possible_metadata[i].permissions.resize(len_perm);
        ifs.read(&qf.possible_metadata[i].permissions[0], len_perm);
        ifs.read(reinterpret_cast<char*>(&len_time), sizeof(len_time));
        if (len_time > 1000) return;
        qf.possible_metadata[i].timestamp.resize(len_time);
        ifs.read(&qf.possible_metadata[i].timestamp[0], len_time);
        ifs.read(reinterpret_cast<char*>(&qf.possible_metadata[i].size), sizeof(qf.possible_metadata[i].size));
    }
        size_t q;
        ifs.read(reinterpret_cast<char*>(&q), sizeof(q));
        if (q > 10000) { std::cerr << "Unreasonable number of qbits: " << q << std::endl; return; }
        qf.qbits.resize(q);
        for (size_t i = 0; i < q; ++i) {
            ifs.read(reinterpret_cast<char*>(&qf.qbits[i].alpha), sizeof(qf.qbits[i].alpha));
            ifs.read(reinterpret_cast<char*>(&qf.qbits[i].beta), sizeof(qf.qbits[i].beta));
            ifs.read(reinterpret_cast<char*>(&qf.qbits[i].measured), sizeof(qf.qbits[i].measured));
            ifs.read(reinterpret_cast<char*>(&qf.qbits[i].value), sizeof(qf.qbits[i].value));
    }
    ifs.close();
}

// File operations
void edit_file(QuantumFile& qf) {
    std::cout << "Enter new content: ";
    std::string new_content;
    std::cin.ignore();
    std::getline(std::cin, new_content);
    qf.possible_contents.push_back(new_content);
    std::cout << "Content added.\n";
}

void delete_file(QuantumFile& qf) {
    qf.possible_contents.clear();
    qf.possible_metadata.clear();
    std::cout << "All possible states deleted.\n";
}

void show_states(const QuantumFile& qf) {
    std::cout << "Possible contents:\n";
    for (const auto& c : qf.possible_contents) std::cout << c << "\n";
}

#include <cmath>

// Qbit operations
void create_qbit(QuantumFile& qf) {
    qf.qbits.push_back({1.0, 0.0, false, -1}); // |0> state
    std::cout << "Qbit created in " << qf.name << " at index " << (qf.qbits.size() - 1) << "\n";
} 

void show_qbits(const QuantumFile& qf) {
    std::cout << "Qbits in " << qf.name << ":\n";
    for (size_t i = 0; i < qf.qbits.size(); ++i) {
        const auto& q = qf.qbits[i];
        std::cout << "Qbit " << i << ": |0> amplitude=" << q.alpha << ", |1> amplitude=" << q.beta
                    << ", measured=" << q.measured << ", value=" << q.value << "\n";
    
    }
}

void hadamard(Qbit& q) {
    double a = q.alpha, b = q.beta;
    q.alpha = (a + b) / std::sqrt(2);
    q.beta = (a - b) / std::sqrt(2);
}

void apply_hadamard(QuantumFile& qf) {
    if (qf.qbits.empty()) { std::cout << "No Qbits to apply Hadamard.\n"; return; }
    std::cout << "Enter Qbit: ";
    size_t idx;
    std::cin >> idx;
    std::cin >> idx;
    if (idx >= qf.qbits.size()) { std::cout << "Invalid index.\n"; return; }
    hadamard(qf.qbits[idx]);
    std::cout << "Hadamard applied to Qbit " << idx << ".\n";
}

void measure_qbit(Qbit& q) {
    if (q.measured) { std::cout << "Qbit already measured: value=" << q.value << "\n"; return;}
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution d(q.beta * q.beta);
    q.value = d(gen) ? 1 : 0;
    q.measured = true;
    q.alpha= (q.value == 0) ? 1.0 : 0.0;
    q.beta = (q.value == 1) ? 1.0 : 0.0;
    std::cout << "Qbit measured: value=" << q.value << "\n";
}

void measure_qbit_cli(QuantumFile& qf) {
    if (qf.qbits.empty()) { std::cout << "No qbits to measure.\n"; return; }
    std::cout << "Enter Qbit index: ";
    size_t idx;
    std::cin >> idx;
    if (idx >= qf.qbits.size()) { std::cout << "Invalid index.\n";}
    measure_qbit(qf.qbits[idx]);
}

int main() {
    QuantumFile fileA{"fileA", {"initA"}, {}, {}, "", {}};
    QuantumFile fileB{"fileB", {"initB"}, {}, {}, "", {}};
    std::mutex mtxA, mtxB;
    Entanglement ent;
    bool running = true;
    while (running) {
        std::cout << "\nQuantumFS CLI:\n";
        std::cout << "1. Evolve fileA\n2. Edit fileA\n3. Delete fileA\n4. Measure fileA\n5. Show fileA states\n";
        std::cout << "6. Evolve fileB\n7. Edit fileB\n8. Delete fileB\n9. Measure fileB\n10. Show fileB states\n";
        std::cout << "11. Entangle\n12. Disentangle\n13. Check entanglement integrity\n14. Save\n15. Load\n16. Exit\nChoose an option: ";
        std::cout << "17. Create qbit in fileA\n18. Create qbit in fileB\n19. Show qbits in fileA\n20. Show qbits in fileB\n";
        std::cout << "21. Hadamard on qbit in fileA\n22. Hadamard on qbit in fileB\n23. Measure qbit in fileA\n24. Measure fileB qbit\n";
        std::cout << "16. Exit\nChoose an option: ";
        int choice;
        std::cin >> choice;
        switch (choice) {
            case 1: {
                LogicGate gate = get_user_gate();
                bool cond1 = true, cond2 = true;
                std::vector<std::thread> threads;
                for (int i = 0; i < 5; ++i)
                    threads.emplace_back(evolve_state, std::ref(fileA), std::ref(mtxA), i, gate, cond1, cond2);
                for (auto& t : threads) t.join();
                std::cout << "Evolution complete.\n";
                break;
            }
            case 2: edit_file(fileA); break;
            case 3: delete_file(fileA); break;
            case 4: measure(fileA); std::cout << "Measured content: " << fileA.observed_content << "\n"; break;
            case 5: show_states(fileA); break;
            case 6: {
                LogicGate gate = get_user_gate();
                bool cond1 = true, cond2 = true;
                std::vector<std::thread> threads;
                for (int i = 0; i < 5; ++i)
                    threads.emplace_back(evolve_state, std::ref(fileB), std::ref(mtxB), i, gate, cond1, cond2);
                for (auto& t : threads) t.join();
                std::cout << "Evolution complete.\n";
                break;
            }
            case 7: edit_file(fileB); break;
            case 8: delete_file(fileB); break;
            case 9: measure(fileB); std::cout << "Measured content: " << fileB.observed_content << "\n"; break;
            case 10: show_states(fileB); break;
            case 11: entangle(ent, fileA, fileB); std::cout << "Files entangled.\n"; break;
            case 12: disentangle(ent); std::cout << "Files disentangled.\n"; break;
            case 13:
                if (check_entanglement_integrity(ent))
                    std::cout << "Entanglement integrity OK.\n";
                else
                    std::cout << "Entanglement integrity BROKEN!\n";
                break;
            case 14: serialize(fileA, "fileA.bin"); serialize(fileB, "fileB.bin"); std::cout << "Saved.\n"; break;
            case 15: deserialize(fileA, "fileA.bin"); deserialize(fileB, "fileB.bin"); std::cout << "Loaded.\n"; break;
            case 17: create_qbit(fileA); break;
            case 18: create_qbit(fileB); break;
            case 19: show_qbits(fileA); break;
            case 20: show_qbits(fileB); break;
            case 21: apply_hadamard(fileA); break;
            case 22: apply_hadamard(fileB); break;
            case 23: measure_qbit_cli(fileA); break;
            case 24: measure_qbit_cli(fileB); break;
            case 16: running = false; break;
            default: std::cout << "Invalid option.\n";
        }
    }
    return 0;
}
