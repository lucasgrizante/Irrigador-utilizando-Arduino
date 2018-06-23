// Programa para projeto cuidador de planta indoor
#include <DS3232RTC.h>
#include <Streaming.h>
#include <Time.h>
#include <LiquidCrystal.h>
 
#define pino_higrometro A0
#define pino_bomba 6
#define pino_nivel 7
#define botaoSelect 8
#define botaoIrrigar 9
#define botaoVoltar 10
#define botaoSubirDescer 13
#define valorMinimoLigarBomba 700
 
int modo = 0; // 0 = manual, 1 = automatico, 2 = no horario
int menu = 0; // 0 = inicio, 1 = Menu
int submenu = 0; //
int selecionarHora = 0; // 0 = hora, 1 = minuto
 
int horario_hora = 12;
int horario_minuto = 0;
 
int horas;
int minutos;
int segundos;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Indica pinos do display LCD (padrão, não dá pra mudar)
 
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
 
  pinMode(pino_higrometro, INPUT);
  pinMode(botaoSubirDescer, INPUT);
  pinMode(botaoVoltar, INPUT);
  pinMode(botaoIrrigar, INPUT);
  pinMode(botaoSelect, INPUT);
  pinMode(pino_bomba, OUTPUT);
 
 
  //Sincroniza a biblioteca Tima com o RTC a cada 5 minutos
  setSyncProvider(RTC.get);
  Serial << F("Sincronizando com o RTC...");
  if (timeStatus() != timeSet) Serial << F(" Falha!");
  Serial << endl;
  escreve_lcd("Irrigador Indoor", 0, "Versao 1.0", 3);
  delay(5000);
 
}
 
void loop() {  
  horas = hour(); //toInt()
  minutos = minute();//.toInt();
  segundos = second();//.toInt();
 
  int valorHigrometro = analogRead(pino_higrometro);
  Serial.print("Valor Higrometro: ");
  Serial.print(valorHigrometro);
  Serial.print(" Hora Atual: ");
  Serial.print(horas);
  Serial.print(" Minuto Atual: ");
  Serial.print(minutos);
  Serial.print(" Segundo Atual: ");
  Serial.print(segundos);
  Serial.println("");
  if (valorHigrometro > valorMinimoLigarBomba && modo == 1) // modo = automatico
  {
    while(analogRead(pino_higrometro) > valorMinimoLigarBomba-200)
      ligaBomba(500); // Liga a bomba por 2 segundos
  }
 
 
  if (digitalRead(botaoIrrigar) == HIGH && modo == 0)
  {
    ligaBomba(500); // liga a bomba por 0.5 segundo.
    // qnd o loop seguir, se continuar pressionado, liga por mais 0.5s ... e por ai vai
  }
 
  if(horas == horario_hora && minutos == horario_minuto && segundos < 15 && modo == 2) // esta no modo horário, na hora certa, e os segundos é < 10 (pra nao regar 60 segundos)
  {
    // observação:
    // nao necessariamente irá ligar no segundo "0", tudo depende dos delays de quando vai entrar na proxima iteração do loop, mas acho que não soma 2 segundos nos delays de verificação
    while(analogRead(pino_higrometro) > valorMinimoLigarBomba-200)
      ligaBomba(500); // Liga a bomba por 0.5
  }
 
  if (digitalRead(botaoSelect) == HIGH)
  {
    if(menu == 0)
    {
      menu = 1;
      submenu = modo;    
    }
    else if(menu == 1)
    {
      modo = submenu; // quando o menu está em alterar modo, e a pessoa apertar o select, vai mudar o modo
      if(modo == 2) // se estiver no modo horario, ir para o menu 2 (que mostra o texto pra definir a hora)
      {
        menu = 2;      
      }
      else
      {
        menu = 0;
      }
    }
    else if(menu == 2)
    {
      selecionarHora++;
      delay(300);
      if(selecionarHora > 1) // ja arrumou a hora e o minuto
      {
        selecionarHora = 0;
        menu = 0; // voltar ao menu "inicio"
      }
    }
    delay(150);
  }
 
    if (digitalRead(botaoVoltar) == HIGH)
    {
    menu = (menu - 1 < 0 ? 0 : menu - 1);
    submenu = modo;
    selecionarHora = 0;
    delay(200);
    }
 
 
  if (digitalRead(botaoSubirDescer) == HIGH)
  {
    if(menu == 1)
    {
      submenu = (submenu+1 > 2 ? 0 : submenu+1);
    }
    else if(menu == 2)
    {
      if(selecionarHora == 0)
      {
        horario_hora = (horario_hora+1 > 23 ? 0 : horario_hora+1);
      }
      else
      {
        // se nao for hora, é minuto
        horario_minuto = (horario_minuto+1 > 59 ? 0 : horario_minuto+1);
      }
    }
    delay(150);
  }
 
  lcd.clear();
 
 
  // exibe no display as informações
  switch (menu)
  {
  case 0:
    if(modo == 0)
    {
      escreve_lcd("Modo: MANUAL", 0, "Alterar Modo", 0);
      delay(150);    
    }
    else if(modo == 1)
    {
      escreve_lcd("Modo: AUTOMATICO", 0, "Alterar Modo", 0);
      delay(150);      
    }
    else
    {
      escreve_lcd("Modo: HORARIO", 0, "Alterar Modo", 0);
      delay(150);    
    }
    break;
    case 1:
    switch(submenu)
    {
      case 0:
        escreve_lcd("Alterar Modo", 0, "MANUAL", 0);
        delay(150);
        break;
      case 1:
        escreve_lcd("Alterar Modo", 0, "AUTOMATICO", 0);
        delay(150);
        break;
      case 2:
        escreve_lcd("Alterar Modo", 0, "HORARIO", 0);
        delay(150);
        break;
      default:
        break;
    }
      break;
  case 2:
    {
      String texto = getTextoHoraSelecionar();
        Serial.println(texto);
    if(selecionarHora == 0)
      escreve_lcd("Definir hora-HH", 0, texto, 0);
    else
      escreve_lcd("Definir hora-MM", 0, texto, 0);
    delay(150);
   }
    break;  
  default:
    break;
  }
 
 
 
}
 
void escreve_lcd(String texto, int x1, String texto2, int x2)
{
  lcd.clear();
  lcd.setCursor(x1, 0);
  lcd.print(texto);
  lcd.setCursor(x2, 1);
  lcd.print(texto2);
}
 
 
String getTextoHoraSelecionar()
{
  String texto = "";
  if(horario_hora < 10)
  {
    texto += "0";
  }
  texto+= horario_hora;
  texto+= ":";
  if(horario_minuto < 10)
  {
    texto += "0";
  }
  texto+= horario_minuto;
  texto+= "  HH/MM";
  return texto;
}
 
void ligaBomba(int tempo)
{
  escreve_lcd("Irrigando...", 0, " ", 0);
  digitalWrite(pino_bomba, HIGH);
  delay(tempo);
  digitalWrite(pino_bomba, LOW);
}