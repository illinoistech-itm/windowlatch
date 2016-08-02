
#include <AESLib.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Crc16.h>
#include <stdio.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x46, 0x77  
};
IPAddress ip(192, 168, 1, 177);
Crc16 crc; 
char valuetoComp= 'M';
// telnet defaults to port 23
EthernetServer server(23);
boolean gotAMessage = false; // whether or not you got a message from the client yet
uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // start the Ethernet connection:
  Serial.println("Trying to get an IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  ip = Ethernet.localIP();
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(ip[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
  // start listening for clients
  server.begin();
  

}

void loop() {
  // wait for a new client:
  
  EthernetClient client = server.available();
  char data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  // when the client sends the first byte, say hello:
  if (client) {
    if (!gotAMessage) {
      Serial.println("We have a new client");
      client.println("Hello, client!");
      gotAMessage = true;
    }

    // read the bytes incoming from the client:
   char thisChar;
    for(int i=0;i<19;i++){
      thisChar = client.read();
      Serial.print("readed: ");
      Serial.println(thisChar);
      data[i]=thisChar;
      if(i==18){
      aes128_dec_single(key, data);
      
      //Serial.print("desencript:");
      //Serial.println(data);
      }
      }
      //aterar as informacoes adicionando um dado novo
      data[1]='8';

      unsigned short verify = calcrc((char*)data, 18);   //USAR A MESMA FUNÇÃO, POIS DIVIDINDO PELO POLINOMIO 0x1021 -> CRC DEVERÁ SER 0. VETOR msg[] = MENSAGEM RECEBIDA.

      
     if(verify==0){  //SE CRC=0, LOGO NENHUM DADO FOI CORROMPIDO.
 
     Serial.println("\nNENHUM DADO FOI CORROMPIDO."); 
    }
    else{
    Serial.println("\nERROR: CORRUPTED DATA"); 
    }

    // echo the bytes back to the client:
 
    // echo the bytes to the server as well:
    Ethernet.maintain();
  }
}
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
