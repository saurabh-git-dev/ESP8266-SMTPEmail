#include <ESP8266WiFi.h>

#define ssid "Your SSID"  // Wifi Name
#define password "Your Pass"  // Wifi Password

const char* host = "smtp.mailgun.org";   // Smtp Server name
const int port = 587;                    // Smtp port

// Base 64 Encoded Email and Password of sender.
const char* senderEncodedEmail = "base64 encoded email";
const char* senderEncodedPassword = "base64 encoded password";

// Recipient address
const char* Recipient = "mail@mail.com";

// From Name and Email.Sender Name will be shown at top of the message at receiver end.
const char* From = "POSTMASTER <postmaster@sandboxa3018ebf82f2439291f4e8c882ea39ee.mailgun.org>";

// Subject
const char* Subject = "ESP8266 EMAIL TESTING";

// HTML Message
const char HTML_EMAIL_TEMPLATE[] PROGMEM = R"=====(
<!doctype html>
<html lang="en">

<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Learning Electronic TESTING</title>
<meta name="viewport" content="width=device-width, initial-scale=1.0" />

<style>
h1{
    margin:auto;
    color:#07B8B4;
    text-align:center;
}


</style>

</head>

<body style="margin: 0; padding: 0;">

    <h1>Learning Electronic</h1>
    <p>Hello Visitor,</p>

    <p><strong>Thank You for visiting learning electronic blog.\n</Strong></p>
    <p style="color:#a1a1a1;text-align:center;margin-top:10px;"><strong>Learning Electronic</Strong></p>

</body>

</html>

)=====";

// Creating WifiClient Object with name SMTPClient
WiFiClient SMTPClient;

// Function for sending email
void sendSmtpEmail(const char** from ,const char** recipient ,const char** subject);
// function for getting response back from server for input commands
byte Response();


void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to :");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.printf("\n[Connection Established with : %s]\n", ssid);

  // Calling Email Function . It will print server response on Serial Monitor
  sendSmtpEmail(&From, &Recipient, &Subject);

}

void loop() {


}


void sendSmtpEmail(const char** from ,const char** recipient ,const char** subject){

  Serial.printf("\nConnecting to %s ... ", host);
  // Checking for connection with SMTP Server
  if (SMTPClient.connect(host, port))
  {
    Response();
  }
  else{
      Serial.println("Failed");
      return 0;
  }

  // You will get 220 Response Code for successful connection.
  // In my case  "220 Mailgun Influx ready"

  SMTPClient.println("HELO");
  if(!Response()){
    Serial.println("Failed");
  }

  // You will receive 250 response with some text.
  // In my case "250 smtp-out-n05.prod.us-east-1.postgun.com"


  SMTPClient.println("AUTH LOGIN");
  if(!Response()){
    Serial.println("Failed");
  }

  // You will get 334 response . It tells that you are ready to send you base64 encoded username
  // In my case response is "334 VXNlcm5hbWU6"


  SMTPClient.println(senderEncodedEmail);
  if(!Response()){
    Serial.println("Failed");
  }

  // You will get 334 response . It tells that you are ready to send you base64 encoded password
  // In my case response is "334 UGFzc3dvcmQ6"

  SMTPClient.println(senderEncodedPassword);
  if(!Response()){
    Serial.println("Failed");
  }    

  // If authentication is successfull you will get response 235 which means Authentication successful.
  // In my case "235 Authentication successful"


  SMTPClient.printf("MAIL FROM: %s\n",*from);
  if (!Response()) {
    Serial.println("Failed");
  }

   // You will get response 250 for OK.
   //In my case     "250 Sender address accepted"



  SMTPClient.printf("RCPT TO: %s\n",*recipient);
  if (!Response()){
    Serial.println("Failed");
  }

  // You will get 250 response.
  // In my case  "250 Recipient address accepted"

  SMTPClient.println(F("DATA"));
  if (!Response()){
    Serial.println("Failed");
  }

  // You will get response 354 which means you are ready to send you data.
  // In my case "354 Continue"


  SMTPClient.printf("To: %s\n",*recipient);
  SMTPClient.printf("From: %s\n",*from);  
  SMTPClient.println("Mime-Version: 1.0");
  SMTPClient.println("Content-Type: Text/HTML; charset=ISO-8859-1");
  SMTPClient.println("Content-Transfer-Encoding: 7bit");
  SMTPClient.printf("Subject: %s\n",*subject);

  //SMTPClient.println(F("This message is sent from ESP8266.\n"));

  // It is neccessary to convert MAIL_TEMPLATE into String. It can't be send directly.
  String body = HTML_EMAIL_TEMPLATE;
  SMTPClient.println(body);

  SMTPClient.println(F("."));
  if (!Response()) {
     Serial.println("Failed");
  }

  // If everything is Ok you will receive successful response code 250.
  // In my case  "250 Great success"

  Serial.println("Successful... Check Your Mail!!!!!!!");
  
}


byte Response()
{
  byte responseCode;
  byte responseByte;
  int Counter = 0;

  while (!SMTPClient.available()) 
  {
    delay(1);
    Counter++;
    //Wating till 3 seconds
    if (Counter > 30000) 
    {
      SMTPClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  responseCode = SMTPClient.peek();
  while (SMTPClient.available())
  {
    responseByte = SMTPClient.read();
    Serial.write(responseByte);
  }

  return 1;
}
