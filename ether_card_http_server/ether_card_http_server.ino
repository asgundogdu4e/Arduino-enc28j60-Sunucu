#include <EtherCard.h>
#include <TM1637Display.h>

#define STATIC 1

#if STATIC
static byte ipAddr[] = { 
  192,168,1,244 };
static byte gwAddr[] = { 
  192,168,1,1 };
#endif

const int CLK = 3; //TM1637Display CLK pini
const int DIO = 2; //TM1637Display DIO pini
int numCounter = 0;
TM1637Display display(CLK, DIO); //TM1637Display Ayarları

static byte mymac[] = { 
  0x74,0x69,0x69,0x2D,0x30,0x3A };
byte Ethernet::buffer[500];

BufferFiller bfill;
int netice = 0;

static word gonder2() {
  bfill = ether.tcpOffset();                      
  bfill.emit_p(PSTR(                              
  "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "<!DOCTYPE html>"
    "<html lang='tr'>"
    "<head>"
    "  <meta charset='UTF-8'/>"
    "  </head>"
    "  <body>"
    "<div>"
    "$D"   
    "</div>"    
    "  </body>"
    "  </html>"    
    ),netice);
  return bfill.position();                        
}

static word gonder() {
  bfill = ether.tcpOffset();                      
  bfill.emit_p(PSTR(                              
  "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    "  <meta charset='UTF-8'/>"
    "$D"   
    ),netice);
  return bfill.position();                        
}

// Talep Şekli: "http://192.168.1.244/?sayi1=52&sayi2=33"
void setup(){
  Serial.begin(9600);
  display.setBrightness(0x0a); //Parlaklık ayarı
  display.showNumberDec(0);
  Serial.println("[Ayarlar:]\n-----------------------------");
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    Serial.println( "Ethernet controller a ulaşılamıyor!");
#if STATIC
  ether.staticSetup(ipAddr, gwAddr);
#else
  if (!ether.dhcpSetup())
    Serial.println("DHCP'den IP alınamadı.");
#endif
  Serial.println(F("\n[Sunucu]"));
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip); 
  ether.printIp("DNS: ", ether.dnsip); 
  display.showNumberDec(0);  
}
void loop(){
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);

  if (pos)  {
    bfill = ether.tcpOffset();
    char* data = (char *) Ethernet::buffer + pos;
    Serial.println("*****************************************************************************");
    String str = data;
    int sayi1Bas = str.indexOf("sayi1="); 
    int sayi2Bas = str.indexOf("&sayi2=");  
    int http11Bas = str.indexOf(" HTTP/1.1"); 
    String sayi1 = str.substring(sayi1Bas + 6, sayi2Bas);   
    String sayi2 = str.substring(sayi2Bas + 7, http11Bas);       
    netice = sayi1.toInt() + sayi2.toInt() ;
    Serial.print("sayi1: ");
    Serial.println(sayi1);    
    Serial.print("sayi2: ");
    Serial.println(sayi2);    
    ether.httpServerReply(gonder());
    display.showNumberDec(netice);    
    display.showNumberDec(sayi1.toInt());
    delay(500);
    display.showNumberDec(sayi2.toInt());
    delay(500);
    Serial.print("Netice: ");
    Serial.println(netice);    
    Serial.println("*****************************************************************************");
  }
}
