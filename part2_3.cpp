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
    cout << "--- SegWit (B' -> C') ---" << endl;

    string addrB = clean(exec("bitcoin-cli getaddressesbylabel \"b_segwit\" | grep -oE '[2mn][a-km-zA-HJ-NP-Z1-9]{25,34}' | head -1"));
    string addrC = clean(exec("bitcoin-cli getaddressesbylabel \"c_segwit\" | grep -oE '[2mn][a-km-zA-HJ-NP-Z1-9]{25,34}' | head -1"));

    string unspent = exec(("bitcoin-cli listunspent 1 9999999 '[\"" + addrB + "\"]'").c_str());
    size_t txid_pos = unspent.find("\"txid\": \"") + 9;
    string u_txid = unspent.substr(txid_pos, 64);
    size_t vout_pos = unspent.find("\"vout\": ") + 8;
    string u_vout = unspent.substr(vout_pos, unspent.find(",", vout_pos) - vout_pos);

    cout << "\n[1] to create tx" << endl;
    string create_cmd = "bitcoin-cli createrawtransaction \"[{\\\"txid\\\":\\\"" + u_txid + "\\\",\\\"vout\\\":" + u_vout + "}]\" \"{\\\"" + addrC + "\\\":4.0, \\\"" + addrB + "\\\":0.9999}\"";
    string raw_hex = clean(exec(create_cmd.c_str()));
    cout << "Hex: " << raw_hex << endl;

    cout << "\n[2] to sign the tx" << endl;
    string signed_json = exec(("bitcoin-cli signrawtransactionwithwallet " + raw_hex).c_str());
    size_t hex_start = signed_json.find("\"hex\": \"") + 8;
    string signed_hex = signed_json.substr(hex_start, signed_json.find("\"", hex_start) - hex_start);


    cout << "\n[3] broadcast" << endl;
    string final_txid = clean(exec(("bitcoin-cli sendrawtransaction " + signed_hex + " 0").c_str()));
    
    if (final_txid.length() == 64) {
        cout << "Success! Final TXID: " << final_txid << endl;
      
        cout << "\n[4] to decode:" << endl;
        cout << exec(("bitcoin-cli decoderawtransaction " + signed_hex).c_str()) << endl;
    } else {
        cout << "Error: " << final_txid << endl;
    }

    return 0;
}