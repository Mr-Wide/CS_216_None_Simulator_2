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

string get_json_val(const string& json, const string& key) {
    size_t keyPos = json.find("\"" + key + "\"");
    if (keyPos == string::npos) return "";
    size_t start = json.find(": \"", keyPos) + 3;
    size_t end = json.find("\"", start);
    return json.substr(start, end - start);
}

int main() {
    cout << "--- Program 2 (B -> C Chain) ---" << endl;
    exec("loadwallet \"part1\"");

    // 1. Fetch Addresses for B and C
    string addr_b = exec("getaddressesbylabel \"B\" | jq -r 'keys[0]'");
    string addr_c = exec("getaddressesbylabel \"C\" | jq -r 'keys[0]'");
    cout << "Target Address B: " << addr_b << endl;

    // 2. Show listunspent for Address B
    cout << "\n--- Listing UTXO for Address B ---" << endl;
    string utxo_list = exec(("listunspent 1 999 '[\"" + addr_b + "\"]'").c_str());
    cout << utxo_list << endl; 

    // Extracting values for transaction creation
    string txid = get_json_val(utxo_list, "txid");
    size_t voutPos = utxo_list.find("\"vout\": ") + 8;
    string vout = utxo_list.substr(voutPos, utxo_list.find(",", voutPos) - voutPos);
    size_t amtPos = utxo_list.find("\"amount\": ") + 10;
    string amt_str = utxo_list.substr(amtPos, utxo_list.find(",", amtPos) - amtPos);
    double total_amt = stod(amt_str);

    // 3. Create Transaction B -> C
    double send_amt = 5.0;
    double change_amt = total_amt - send_amt - 0.001;

    string inputs = "[{\"txid\":\"" + txid + "\",\"vout\":" + vout + "}]";
    string outputs = "{\"" + addr_c + "\":" + to_string(send_amt) + ",\"" + addr_b + "\":" + to_string(change_amt) + "}";
    string raw_hex = exec(("createrawtransaction '" + inputs + "' '" + outputs + "'").c_str());

    // 4. Sign the transaction
    string signed_json = exec(("signrawtransactionwithwallet " + raw_hex).c_str());
    string signed_hex = get_json_val(signed_json, "hex");

    // 5.Decode and Analyze ScriptSig (Response)
    cout << "\n--- Decoded ScriptSig (Response/Unlocking Script) ---" << endl;
    string decoded = exec(("decoderawtransaction " + signed_hex).c_str());
  
    size_t vinPos = decoded.find("\"vin\"");
    size_t scriptSigPos = decoded.find("\"asm\"", vinPos);
    size_t start = decoded.find(": \"", scriptSigPos) + 3;
    size_t end = decoded.find("\"", start);
    cout << decoded.substr(start, end - start) << endl;

    // 6.Broadcast
    string final_txid = exec(("sendrawtransaction " + signed_hex + " 0").c_str());
    cout << "\nFinal TXID (B -> C): " << final_txid << endl;

    return 0;
}