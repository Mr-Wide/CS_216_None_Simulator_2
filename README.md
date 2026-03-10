# Submission for BlockChain Assignment 2: Bitcoin — A Hands On Approach

## Team Members
- **Sriniketan Prabandham** — 240001052  
- **Khaled** — 240001041  
- **Malthum Thanush** — 240005051  
- **Vivek Sahu** — 240005028  

---

# Instructions Of Execution

---

# Phase 1

### 1. Create the Makefile
```bash
cat <<EOF > Makefile
CXX = g++
all: part1_raw part1_chain

part1_raw: part1_raw.cpp
	$(CXX) part1_raw.cpp -o part1_raw

part1_chain: part1_chain.cpp
	$(CXX) part1_chain.cpp -o part1_chain

clean:
	rm -f part1_raw part1_chain
EOF
```
ls -a ~/.bitcoin/bitcoin.conf
cat ~/.bitcoin/bitcoin.conf

make



./part1_raw
./part1_chain
bitcoin-cli sendrawtransaction "<output hash here>"
bitcoin-cli generatetoaddress 1 $(bitcoin-cli getnewaddress)
bitcoin-cli listunspent

Debugger structure
[Push Signature Opcode][Signature][Push PubKey Opcode][PubKey][scriptPubKey]
ex : btcdeb 473044022028e82ff4424515e741f508b0a3c5ba04000621a64ff546f17caa18ff8c82086102200500447128104cb64ef23761938da43981634686b174269dbeb7dbdd67770569129aa51d1dilebd0d58a76a91490245732181887494all4a64a4a5colf72a8219acBlac

Then :
step  # Push Signature
step  # Push Public Key
step  # OP_DUP
step  # OP_HASH160
step  # Push Target Hash
step  # OP_EQUALVERIFY
step  # OP_CHECKSIG
