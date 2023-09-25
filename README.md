# r2web3
A Web3 library intended to be used for microcontrollers. Has at this point only been tested on ESP8266 boards.

This project is - in its current stage - incomplete and more of means serving me to learn about blockchain development. Here's a list of a few thing that's on the initial roadmap:
 * Parse errors correctly
 * Clean up network requests
 * Unit tests
 * ABI encoding (existing, but incomplete)
 * ABI decoding (not implemented)
 * Network clients for other platforms (e g ESP32, Arduino etc)
 * Simplification of contract calls
 * Various JSON-RPC implementations (e g gas estimation, receipt retrieval etc)
 * EIP-155 support

## Installation
```
$ cd ~/Arduino/libraries
$ git clone https://github.com/TordWessman/r2web3.git
```
Remember to restart Arduino IDE

## Examples
Here's a blob of example code.

```
#include <ESP8266WiFi.h>
#include <r2web3.h>

using namespace blockchain;

#define SSID "<your network>"
#define WIFI_PASSWORD "<w-lan password>"
#define PRIVATE_KEY "<private key>"
#define CONTRACT_ADDRESS "<contract address>"

Account account(PRIVATE_KEY);
Address contractAddress(CONTRACT_ADDRESS);
ESP8266Network networkFacade; // Use the ESP8266 network stack.
Chain chain("http://<computer ip-address>:8545", &networkFacade); //Local Ganache instance

void(* restartDevice) (void) = 0; 

void setup() {

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  WiFi.begin(SSID, WIFI_PASSWORD);

  int connectionAttempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    delay(500);
    if(connectionAttempts++ > 10) {
      restartDevice();
    }
  }
  
  Serial.println();

  Serial.println("Connecting to blockchain...");
  if(chain.Start()) {
    Serial.println("...done");
  } else {
    Serial.println("...failed");
    restartDevice();
  }
}


/// Example of a transfer to a specific address
void makeTransfer() {
  Address sendToAddress("0x3e415b89e5406c4d13717D434A2021bA97b4B5E7");
  BigUnsigned gasPrice = chain.GetGasPrice().Value();
  BigUnsigned sendAmount = BigUnsigned(random(1000,10000));
  uint32_t gasLimit = 6721975;
  chain.Send(&account, sendToAddress, sendAmount, gasPrice, gasLimit);
}

/// Call contract without creating a new block
void viewCall() {
  ContractCall call("add", {ENC(10u, "uint32"), ENC({ENC(20u), ENC(30u)}, "uint32[]")}); //Equivalent to method signature 'add(uint32,uint32[])'
  
  Result<TransactionResponse> result = chain.ViewCall(&call, contractAddress);
  
  if (result.HasValue()) {
    Serial.print("Result: "); Serial.println(result.Value().Result());
  } else {
    Serial.print("Failure: "); Serial.println(result.ErrorCode());
  }
}

/// Contract call, creating a block.
void mutateCall() {
    ContractCall contractCall("setFoo", {ENC((uint32_t)random(1000,10000))}); //Equivalent to method signature 'setFoo(uint256)'
    BigUnsigned gasPrice(1234567); // Could be calculated using 'eth_estimateGas' once implemented.
    BigUnsigned sendAmount = BigUnsigned(0u);
    uint32_t gasLimit = 6721975;
    Result<TransactionResponse> contractCallResponse = chain.Send(&account,     contractAddress, sendAmount, gasPrice, 6721975, &contractCall);
}

void loop() {
    makeTransfer();
    viewCall();
    mutateCall();
    delay(1000);
}
```