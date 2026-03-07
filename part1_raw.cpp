#include <iostream>
#include <string>
#include <array>
#include <memory>

using namespace std;

string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    string command = "bitcoin-cli -regtest " + string(cmd) + " 2>&1";
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe) return "Error";
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
}

int main() {
    cout << "--- Part 1: The Setup ---" << endl;
    exec("loadwallet \"part1\"");

    
    string addr_a = exec("getnewaddress \"A\" \"legacy\"");
    string addr_b = exec("getnewaddress \"B\" \"legacy\"");
    string addr_c = exec("getnewaddress \"C\" \"legacy\"");
    
    cout << "Address A: " << addr_a << endl;
    cout << "Address B: " << addr_b << endl;
    cout << "Address C: " << addr_c << endl;

    
    string dummy = exec("getnewaddress \"dummy\" \"legacy\"");
    exec(("generatetoaddress 101 " + dummy).c_str());
    exec(("sendtoaddress " + addr_a + " 25").c_str());
    exec(("generatetoaddress 1 " + dummy).c_str());

    
    string txid = exec(("listunspent 1 999 '[\"" + addr_a + "\"]' | jq -r '.[0].txid'").c_str());
    string vout = exec(("listunspent 1 999 '[\"" + addr_a + "\"]' | jq -r '.[0].vout'").c_str());

    
    string inputs = "[{\"txid\":\"" + txid + "\",\"vout\":" + vout + "}]";
    string outputs = "{\"" + addr_b + "\":10,\"" + addr_a + "\":14.99}";
    string raw_hex = exec(("createrawtransaction '" + inputs + "' '" + outputs + "'").c_str());

    
    cout << "\n--- Decoded ScriptPubKey (Challenge) ---" << endl;
    cout << exec(("decoderawtransaction " + raw_hex + " | jq -r '.vout[0].scriptPubKey.asm'").c_str()) << endl;

    
    string signed_json = exec(("signrawtransactionwithwallet " + raw_hex).c_str());
    
    // following is done to avoid shell errors we were getting before
    string signed_hex = "";
    size_t hex_start = signed_json.find("\"hex\": \"");
    if (hex_start != string::npos) {
        hex_start += 8; // move past "hex": "
        size_t hex_end = signed_json.find("\"", hex_start);
        signed_hex = signed_json.substr(hex_start, hex_end - hex_start);
    }

    if (!signed_hex.empty()) {
        string final_txid = exec(("sendrawtransaction " + signed_hex).c_str());
        cout << "\nFinal TXID (A -> B): " << final_txid << endl;
    } else {
        cout << "\nError: Could not extract signed hex. Raw JSON: " << signed_json << endl;
    }

    return 0;
}
