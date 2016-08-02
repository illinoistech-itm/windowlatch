#include <SPI.h>
#include <Ethernet.h>
#include <AESLib.h> 
#include <Crc16.h>

  char data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  byte datab[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  //the value of the Indetification ID of the other client
  char ValueOfId ='P';
  
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x48, 0x62
};

// ip address of the server if the DHCP atribuition is not working
IPAddress ip(192, 168, 1, 177);

// Enter the IP address of the server you're connecting to:
IPAddress server(192, 168, 1, 163); //por endereco do servidor

Crc16 crc; 

// Initialize the Ethernet client library
// with the IP address and port of the server
// port 23 is default for telnet;
EthernetClient client;

//encrypt key 
uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
int i = 0;

 

void setup() {
  pinMode(A0, INPUT);
  pinMode(2, OUTPUT);  //white   --- read sensor
  pinMode(7, OUTPUT);  // Yellow --- calculate CRC
  pinMode(8, OUTPUT);  //Green   --- Encrypt 
  pinMode(9, OUTPUT);  // Red    --- Error
  // start the Ethernet connection:
  Ethernet.begin(mac); 
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // give the Ethernet shield a second to initialize:
  randomSeed(analogRead(0));
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 23)) {
    Serial.println("connected");
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
   
}

void loop() {
      //call the function readsens
    if(readsens()==1){ 

  while (Serial.available() > 0) {
  
    
  }
    }
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    // do nothing:
    while (true);
  }
}

// function to calculate the CRC
int calcrc(char *ptr, int count)
{
    int  crc;
    char i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}


 int readsens(){
  // read some data from the serial port to trigger the system
 Serial.println("Enter '1' command to start the readings");
    char readed = Serial.read();
    while(readed!='1'){
   readed = Serial.read();   
    }
  if(readed == '1')
  {
    digitalWrite(2, HIGH);// turn led White on to indicate the read of the sensor   
    // read the sensor and map the value
    i = analogRead(A0);
    i = map(i, 0, 1024, 0, 9);

    switch(i)
    {
      case 0: data[0] = '0';

        break;
      case 1: data[0] = '1';

        break;
      case 2: data[0] = '2';

        break;
      case 3: data[0] = '3';

        break;
      case 4: data[0] = '4';
     
        break;
      case 5: data[0] = '5';
     
        break;
      case 6: data[0] = '6';
      
        break;
      case 7: data[0] = '7';
     
        break;
      case 8: data[0] = '8';
    
        break;
      case 9: data[0] = '9';
   
        break;
      default: Serial.println ("ERROR! Sensor not found");

        break;
    }

    int randNumber = random(127);
    data[7]=randNumber;
    Serial.print("Sensor: ");
    Serial.println(data);
    Serial.println("");
     for(int a=0;a<16;a++){
      datab[a]=data[a];
      } 
    delay(1000);
    digitalWrite(7, HIGH);// turn led Yellow on to indicate the calc of the CRC 
    
    unsigned short value = calcrc((char*)datab, 16);
    data[16] = highByte(value);   //Add the 2 bytes of the CRC to the end of the mensage 
    data[17] = lowByte(value); 

    delay(1000);
    digitalWrite(8, HIGH);// turn led green on to indicate the encryption process
    aes128_enc_single(key, data);
    delay(1000);
    
    // add the id number at the end of the string of data after encryption
    data[18]=ValueOfId;
    
    if (client.connected()) {
      
      client.print(data);
      //blink all the four leds to indicate transmition of information
      digitalWrite(2, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      delay(400);
      digitalWrite(2, LOW);
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
      delay(400);
      digitalWrite(2, HIGH);
      digitalWrite(7, HIGH);
      digitalWrite(8, HIGH);
      digitalWrite(9, HIGH);
      delay(400); 
      digitalWrite(2, LOW);
      digitalWrite(7, LOW);
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);    
    }
    //clean the data arrays to next readings 
    for(int j=0;j<16;j++){
      datab[j]=0;
      } 
      for(int k=0;k<16;k++){
      data[k]=0;
      }
    return 1;
  }}

