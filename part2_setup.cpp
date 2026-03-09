#include <iostream>
#include <string>
#include <array>
#include <memory>
using namespace std;

string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "error";
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
    cout << "--- Part 2: SegWit Setup (A -> B) ---" << endl;

    
    string addrA = exec("bitcoin-cli getnewaddress \"a_segwit\" \"p2sh-segwit\"");
    string addrB = exec("bitcoin-cli getnewaddress \"b_segwit\" \"p2sh-segwit\"");
    string addrC = exec("bitcoin-cli getnewaddress \"c_segwit\" \"p2sh-segwit\"");
    
    addrA.erase(addrA.find_last_not_of(" \n\r\t")+1);
    addrB.erase(addrB.find_last_not_of(" \n\r\t")+1);
    addrC.erase(addrC.find_last_not_of(" \n\r\t")+1);

    cout << "Address A: " << addrA << endl;
    cout << "Address B: " << addrB << endl;
    cout << "Address C: " << addrC << endl;


    cout << "Mining 101 blocks to Address A to fund it..." << endl;
    string mine_cmd = "bitcoin-cli generatetoaddress 101 " + addrA;
    exec(mine_cmd.c_str());


    cout << "Sending 10 BTC from A to B..." << endl;
    string fund_cmd = "bitcoin-cli sendtoaddress " + addrB + " 10.0";
    string txid = exec(fund_cmd.c_str());
    txid.erase(txid.find_last_not_of(" \n\r\t")+1);


    exec("bitcoin-cli -generate 1");

    cout << "Final TXID (A -> B): " << txid << endl;
    cout << "Status: Address B is now funded with 10 BTC and confirmed." << endl;

    return 0;
}