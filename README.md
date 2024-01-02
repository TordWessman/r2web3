# r2web3
A Web3 library intended to be used for microcontrollers. Has at this point only been tested on ESP8266 and ESP32 boards.

This project is under development. Here's a list of a few thing that's on the initial roadmap:
 * Parse errors
 * Clean up `Chain`
 * Unit tests
 * ABI encoding (existing, but incomplete)
 * ABI decoding
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
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#ifdef ESP32
#include <WiFi.h>
#endif
#include <r2web3.h>

using namespace blockchain;

#define SSID "<ssid>"
#define WIFI_PASSWORD "<password>"
#define PRIVATE_KEY "<private key>" // Account private key
#define CONTRACT_ADDRESS "<contract address>" // Address to a deployed contract

// Use CA Certificate for https.
// The one below is valid for json-rpc.evm.testnet.shimmer.network.
// Use root certificate unless accessing a local endpoint (e.g. Ganache)
const char cert [] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIDzTCCArWgAwIBAgIQCjeHZF5ftIwiTv0b7RQMPDANBgkqhkiG9w0BAQsFADBa
MQswCQYDVQQGEwJJRTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJl
clRydXN0MSIwIAYDVQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTIw
MDEyNzEyNDgwOFoXDTI0MTIzMTIzNTk1OVowSjELMAkGA1UEBhMCVVMxGTAXBgNV
BAoTEENsb3VkZmxhcmUsIEluYy4xIDAeBgNVBAMTF0Nsb3VkZmxhcmUgSW5jIEVD
QyBDQS0zMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEua1NZpkUC0bsH4HRKlAe
nQMVLzQSfS2WuIg4m4Vfj7+7Te9hRsTJc9QkT+DuHM5ss1FxL2ruTAUJd9NyYqSb
16OCAWgwggFkMB0GA1UdDgQWBBSlzjfq67B1DpRniLRF+tkkEIeWHzAfBgNVHSME
GDAWgBTlnVkwgkdYzKz6CFQ2hns6tQRN8DAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0l
BBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwNAYI
KwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5j
b20wOgYDVR0fBDMwMTAvoC2gK4YpaHR0cDovL2NybDMuZGlnaWNlcnQuY29tL09t
bmlyb290MjAyNS5jcmwwbQYDVR0gBGYwZDA3BglghkgBhv1sAQEwKjAoBggrBgEF
BQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQUzALBglghkgBhv1sAQIw
CAYGZ4EMAQIBMAgGBmeBDAECAjAIBgZngQwBAgMwDQYJKoZIhvcNAQELBQADggEB
AAUkHd0bsCrrmNaF4zlNXmtXnYJX/OvoMaJXkGUFvhZEOFp3ArnPEELG4ZKk40Un
+ABHLGioVplTVI+tnkDB0A+21w0LOEhsUCxJkAZbZB2LzEgwLt4I4ptJIsCSDBFe
lpKU1fwg3FZs5ZKTv3ocwDfjhUkV+ivhdDkYD7fa86JXWGBPzI6UAPxGezQxPk1H
goE6y/SJXQ7vTQ1unBuCJN0yJV0ReFEQPaA1IwQvZW+cwdFD19Ae8zFnWSfda9J1
CZMRJCQUzym+5iPDuI9yP+kHyCREU3qzuWFloUwOxkgAyXVjBYdwRVKD05WdRerw
6DEdfgkfCv4+3ao8XnTSrLE=
-----END CERTIFICATE-----
)CERT";

Account account(PRIVATE_KEY);
Address contractAddress(CONTRACT_ADDRESS);
ESPNetwork networkFacade(cert); // Use the ESP network stack.
Chain chain("https://json-rpc.evm.testnet.shimmer.network", &networkFacade); // Use ShimmerEVM testnet.

void setup() {

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  WiFi.begin(SSID, WIFI_PASSWORD);

  int connectionAttempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("Connecting to blockchain...");
  if(chain.Start()) {
    Serial.println("...done");
  } else {
    Serial.println("...failed");
  }
}

/// Example of a transfer to a specific address
void makeTransfer() {
  Address sendToAddress("<recipient address>");
  BigNumber sendAmount = BigNumber(random(1000,10000));
  uint32_t gasLimit = 6721975;
  chain.Send(&account, sendToAddress, sendAmount, gasLimit);
}

/// Check balance
void getBalance() {

  Result<BigNumber> balanceResult = chain.GetBalance(account.GetAddress());

  if (balanceResult.HasValue()) {

    // Retrieve tha balance and convert it into a human-friendly decimal string.
    char *balanceDecimalString = balanceResult.Value().GenerateDecimalString();
    Serial.print("Balance: "); Serial.println(balanceDecimalString);
    free(balanceDecimalString);
  }

  // Retrieve the balance of an ERC-20 contract
  Result<BigNumber> contractBalanceResult = chain.GetBalance(account.GetAddress(), contractAddress);
}

/// View call
void viewCall() {
  ContractCall call("add", {ENC(10u, "uint32"), ENC({ENC(20u), ENC(30u)}, "uint32[]")}); // Equivalent to method signature 'add(uint32,uint32[])'
  
  Result<TransactionResponse> result = chain.ViewCall(&call, account.GetAddress(), contractAddress);
  
  if (result.HasValue()) {
    Serial.print("Result: "); Serial.println(result.Value().Result());
  } else {
    Serial.print("Failure: "); Serial.println(result.ErrorCode());
  }
}

/// (State changing) contract call
void mutateCall() {
    ContractCall contractCall("setFoo", {ENC((uint32_t)random(1000,10000))}); // Equivalent to method signature 'setFoo(uint256)'
    BigNumber gasPrice(1234567); // chain.EstimateGas could be used instead.
    BigNumber sendAmount = BigNumber(0u);
    uint32_t gasLimit = 6721975;
    Result<TransactionResponse> contractCallResponse = chain.Send(&account, contractAddress, sendAmount, 6721975, &gasPrice, &contractCall);
}

void loop() {
    makeTransfer();
    viewCall();
    mutateCall();
    getBalance();
    delay(1000 * 60 * 60);
}
```

## Secure connection (HTTPS)
When accessing a HTTP over TLS, one must specify the root certificate for the endpoint when instantiating the `ESPNetwork`.

Please note that `Restart()` method must be called once the WiFi connection has been established and _if_ the device clock has lost synchronization (e g after hybernation).

There are several ways to retrieve the certificate, and here's one:
```
$ openssl s_client -showcerts -verify 5 -connect <endpoint>:443 < /dev/null
```