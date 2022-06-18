/* erro 226 =  ***a resposta inteira não foi recebida dentro do período de tempo limite.
 *  *** exceção de tempo limite de resposta

   A chave:  https://partofthething.com/thoughts/reading-a-tuf-2000m-ultrasonic-flow-meter-with-an-arduino-or-esp8266/

   big endian: https://cadernodelaboratorio.com.br/o-que-significam-as-palavras-little-endian-e-big-endian/

   memcpy:  https://www.delftstack.com/pt/howto/arduino/arduino-memcpy/

   Biblioteca e suas funcoes: https://4-20ma.io/ModbusMaster/group__buffer.html#ga5e932dcb444f23094d6bf74963c5dccc

   Autor : Anderson dos reis Cardoso
   Data: 17/06/2022
   Local: Brasilia-DF

*/

#include <ModbusMaster.h>
//Data Enable (DE) and Receiver Enable (/RE)
#define MAX485_DE      24
#define MAX485_RE_NEG  22

ModbusMaster node;

uint8_t resultMain;

//nesse vetor podemos colocar os enderecos dos slaves da rede para serem lidos
int endereco_Rede_Slaves[] {
  1,      // Endereço Multimedidor pac 3200 Siemens
  //2,
  //3,
  //4
};


//Lembra que os reg. tem ser em HEX, exe: 55 = 37
//nesse vetor podemos colocar os registros do slave que desejamos ler
int vetor_Endereco_Regs[] {
  0x01,   // 0 - Tensão Fase-Neutro L1-N - DEC(01)
  0x4B,   // 1 - Tensão Maxima L1-N      - DEC(75)
  0x91,   // 2 - Tensao minima L1-N      - DEC(145)
  0x83,   // 3 - Tensao media L1-N       - DEC(131)
  0x37,   // 4 - Frequencia Instantanea  - DEC(55)
  0x81,   // 5 - Frequencia Maxima       - DEC(129)
  0xBB,   // 6 - Frequencia Minima       - DEC(187)
};


void preTransmission() {
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}
void postTransmission() {
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup() {
  // Atribui pinos como saída
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // inicializa modo de recebimento
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);

  // Atribui velocidade de comunicação (Baud Rate)
  Serial.begin(9600);
  Serial1.begin(19200, SERIAL_8O1);

  // Callbacks - Permite configurar o transeiver RS485 corretamente
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}
void loop() {
  for ( int i = 0; i < 1; i ++) {
    read_Multimedidor(endereco_Rede_Slaves[i]);
    delay(5000);
  }
}
/*************************************************************
  Realiza a leitura dos dados via RS485 do Multimedidor
*************************************************************/
float read_Multimedidor(uint8_t deviceID) {

  uint8_t j;
  uint16_t _buffer[2], memAux;
  float  dataOut;


  node.begin(deviceID, Serial1);

  Serial.println("************INICIANDO VARREDURA*************");
  Serial.println("--------------------------------------------");
  Serial.print("Slave de Endereço: ");
  Serial.println(deviceID);
  Serial.println("--------------------------------------------");


  for (int i = 0; i < 8; i ++) {
    resultMain = node.readHoldingRegisters(vetor_Endereco_Regs[i], 2); // Realiza a leitura do registrador

    delay(1500);

    if (resultMain == node.ku8MBSuccess) {
      //Serial.println("Sucesso! Processando ...");

      for (j = 0; j < 8; j++ ) {
        _buffer[j] = node.getResponseBuffer(j);
      }

      //Os dados retornados da leitura modbus ser BIG-ENDIAN, ajustamos isso abaixo
      memAux = _buffer[1];
      _buffer[1] = _buffer[0];
      _buffer[0] = memAux;

      //Essa função memcpy = possui 3 argumentos, o primeiro recebe os dados do segundo argumento
      //e o terceiro e o tipo de dado que usará
      memcpy(&dataOut, &_buffer, sizeof(float));

      switch (i) {
        case 0:
          Serial.println("Tensão entre fase e Neutro - L1-N");
          Serial.print(dataOut, 4);   //4 - numero de casa decimais
          Serial.println(" Volts");
          Serial.println("+++++++++++++++++++++++++++++++++++++++++++++");
          Serial.println(" ");
          break;
        case 1:
          Serial.println("Tensão Máxima L1-N");
          Serial.print(dataOut, 4);   //4 - numero de casa decimais
          Serial.println(" Volts");
          Serial.println("+++++++++++++++++++++++++++++++++++++++++++++");
          Serial.println(" ");
          break;
        case 2:
          Serial.println("Tensão Mínima L1-N");
          Serial.print(dataOut, 4);   //4 - numero de casa decimais
          Serial.println(" Volts");
          Serial.println("+++++++++++++++++++++++++++++++++++++++++++++");
          Serial.println(" ");
          break;
        case 3:
          Serial.println("Tensao Média L1-N");
          Serial.print(dataOut, 4);   //4 - numero de casa decimais
          Serial.println(" Volts");
          Serial.println("+++++++++++++++++++++++++++++++++++++++++++++");
          Serial.println(" ");
          break;
        case 4:
          Serial.println("Frequencia Instantanea - HZ");
          Serial.print(dataOut, 4);   //4 - numero de casa decimais
          Serial.println(" Hertz");
          Serial.println("+++++++++++++++++++++++++++++++++++++++++++++");
          Serial.println(" ");
          break;
        case 5:
          Serial.println("Frequencia Maxima - HZ");
          Serial.print(dataOut, 4);   //4 - numero de casa decimais
          Serial.println(" Hertz");
          Serial.println("+++++++++++++++++++++++++++++++++++++++++++++");
          Serial.println(" ");
          break;
        case 6:
          Serial.println("Frequencia Mínima - HZ");
          Serial.print(dataOut, 4);   //4 - numero de casa decimais
          Serial.println(" hertz");
          Serial.println("+++++++++++++++++++++++++++++++++++++++++++++");
          Serial.println(" ");
          break;
      }
    }
  }
}
