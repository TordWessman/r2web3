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

## Secure connection (HTTPS)
When accessing a HTTP over TLS, one must specify the root certificate for the endpoint when instantiating the `ESP8266Network`.

Note that `Restart()` method must be called once WiFi connection has been eshablished.

```
const char publicnode_cert [] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIFczCCBFugAwIBAgIRAIB/WngBnz5zEYRCi1GK3VswDQYJKoZIhvcNAQELBQAw
RjELMAkGA1UEBhMCVVMxIjAgBgNVBAoTGUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBM
TEMxEzARBgNVBAMTCkdUUyBDQSAxUDUwHhcNMjMwOTA4MTc1NDUyWhcNMjMxMjA3
MTc1NDUxWjAZMRcwFQYDVQQDEw5wdWJsaWNub2RlLmNvbTCCASIwDQYJKoZIhvcN
AQEBBQADggEPADCCAQoCggEBAMzydha8aP9OERCU3vLazmgK/bhvKh8muO6f2kTB
5GRScQCvE3gboxQ5dqZjBHrchrdif7OsLKSvBYJPTck0SpdlMIu7gK5NW8e+gcSP
npWrBbVlrXiHdk4AUVDddtZAqj6Z7Q/r2Rcp5a7zOX6bmuPB/KBJp4UgYZDafUTy
AdotqHG+X90z4MyuLZzHsCa2g5nLheMXcRbLVLApbOqza5qKTtj81K/Or/Q2dv2e
SEn47QqIHfyXi3HkIRUnAGzY5SBdTzb6/E6daon6Uwq5mnUYUNeQvAuQn8eSgA0O
MFoIIsBINa1OfzhnhBTaR47gyxyfW+HduUbS3mO/W0AY5tsCAwEAAaOCAocwggKD
MA4GA1UdDwEB/wQEAwIFoDATBgNVHSUEDDAKBggrBgEFBQcDATAMBgNVHRMBAf8E
AjAAMB0GA1UdDgQWBBRxif8/WofIuhPX77HxXwu+O4SK7DAfBgNVHSMEGDAWgBTV
/J4N3x7K3QiXl24rxV/FK/XsuDB4BggrBgEFBQcBAQRsMGowNQYIKwYBBQUHMAGG
KWh0dHA6Ly9vY3NwLnBraS5nb29nL3MvZ3RzMXA1L1pWb0JKaThBUEdRMDEGCCsG
AQUFBzAChiVodHRwOi8vcGtpLmdvb2cvcmVwby9jZXJ0cy9ndHMxcDUuZGVyMCsG
A1UdEQQkMCKCDnB1YmxpY25vZGUuY29tghAqLnB1YmxpY25vZGUuY29tMCEGA1Ud
IAQaMBgwCAYGZ4EMAQIBMAwGCisGAQQB1nkCBQMwPAYDVR0fBDUwMzAxoC+gLYYr
aHR0cDovL2NybHMucGtpLmdvb2cvZ3RzMXA1LzczWVBiNWotRDVnLmNybDCCAQQG
CisGAQQB1nkCBAIEgfUEgfIA8AB2AOg+0No+9QY1MudXKLyJa8kD08vREWvs62nh
d31tBr1uAAABinYlpgkAAAQDAEcwRQIhAIEBtYoTVOlTi9/HwwopMgAM54llQWtC
wcLEScobexRpAiAqnyresocTJqefnIVff2A5XBuiyoo/XJaQerqqhAuZJQB2AHoy
jFTYty22IOo44FIe6YQWcDIThU070ivBOlejUutSAAABinYlpkIAAAQDAEcwRQIh
AJfKchfKxn1025f3dduXMXPwbJGZTp7MlZWwQGLKVfLuAiAGRXZt5dD6mdEHIloi
1MjzvzUcIPQ4HTJISYDxvDK1azANBgkqhkiG9w0BAQsFAAOCAQEApCwg8/Z+gFkG
vcc1P+aoDpRKKGvlusFNEC2KczyGBNNTYoC34UZEklKVdV1DBQFc3GGKTtBNuZW0
8onlT2kqPzfc5X/zckj5uLQEnoJ9mGLnoCtYl/ZMTtU/HXv8HSFGv2bXShrQebiu
cIvDX4nrgJKc+t6SW8gffQhLP8miWopzKNvVaydRuA7gUpCs1593riBh6z3JTvrV
lUWu9FUk93sVIDfYr1/jTnT9s9+o8wTAvn0UjreUvAi3u5wqfkFfjS7SpwdwtE4Y
+Z/GI0b69lBizYfxym+a6jNbTe8XvlrJhOUhAYPDmb5Ny0JdH6jgnRuHq0rVuPLf
+vwQR4IviA==
-----END CERTIFICATE-----
)CERT";

ESP8266Network networkFacade(publicnode_cert);

Chain chain("https://ethereum.publicnode.com", &networkFacade); // Only RPC-urls where the root certificate has been provided can be accessed.

void Setup() {

  // Connect to WiFi ...

  networkFacade.Restart();
} 
``` 
There are several ways to retrieve the certificate, and here's one:
```
$ openssl s_client -showcerts -verify 5 -connect <endpoint>:443 < /dev/null
```