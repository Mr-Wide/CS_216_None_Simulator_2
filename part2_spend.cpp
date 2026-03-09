#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <algorithm>

using namespace std;

string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "";
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

string clean(string s) {
    s.erase(remove(s.begin(), s.end(), '\"'), s.end());
    s.erase(remove(s.begin(), s.end(), '\n'), s.end());
    s.erase(remove(s.begin(), s.end(), '\r'), s.end());
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    return s;
}

int main() {
    cout << "--- SegWit (A' -> B') ---" << endl;

    
    string addrA = clean(exec("bitcoin-cli getaddressesbylabel \"a_segwit\" | grep -oE '[2mn][a-km-zA-HJ-NP-Z1-9]{25,34}' | head -1"));
    string addrB = clean(exec("bitcoin-cli getaddressesbylabel \"b_segwit\" | grep -oE '[2mn][a-km-zA-HJ-NP-Z1-9]{25,34}' | head -1"));

    
    string unspent = exec(("bitcoin-cli listunspent 1 9999999 '[\"" + addrA + "\"]'").c_str());
    size_t txid_pos = unspent.find("\"txid\": \"") + 9;
    string u_txid = unspent.substr(txid_pos, 64);
    size_t vout_pos = unspent.find("\"vout\": ") + 8;
    string u_vout = unspent.substr(vout_pos, unspent.find(",", vout_pos) - vout_pos);

    cout << "\n[1] we first create a tx" << endl;
    string create_cmd = "bitcoin-cli createrawtransaction \"[{\\\"txid\\\":\\\"" + u_txid + "\\\",\\\"vout\\\":" + u_vout + "}]\" \"{\\\"" + addrB + "\\\":5.0, \\\"" + addrA + "\\\":4.9999}\"";
    string raw_hex = clean(exec(create_cmd.c_str()));
    cout << "Hex: " << raw_hex << endl;


    cout << "\n[2] to decode the tx" << endl;
    cout << exec(("bitcoin-cli decoderawtransaction " + raw_hex).c_str()) << endl;


    cout << "\n[3] to sign the tx" << endl;
    string signed_json = exec(("bitcoin-cli signrawtransactionwithwallet " + raw_hex).c_str());
    size_t hex_start = signed_json.find("\"hex\": \"") + 8;
    string signed_hex = signed_json.substr(hex_start, signed_json.find("\"", hex_start) - hex_start);
    cout << "Signed Hex: " << signed_hex << endl;


    cout << "\n[4] to broadcast" << endl;
    string final_txid = clean(exec(("bitcoin-cli sendrawtransaction " + signed_hex + " 0").c_str()));
    
    if (final_txid.length() == 64) {
        cout << "Success! Broadcasted TXID: " << final_txid << endl;
        exec("bitcoin-cli -generate 1");
    } else {
        cout << "Error: " << final_txid << endl;
    }

    return 0;
}