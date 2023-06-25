// C++ code
// Parcial 2 Sistema de Procesamiento de Datos
// Examen Practico domiciliario
// Videla Ribodino Iván Agustín
// Div "D"
// El objetivo de este proyecto es diseñar un sistema de incendio utilizando Arduino que pueda
// detectar cambios de temperatura y activar un servo motor en caso de detectar un incendio.
// Además, se mostrará la temperatura actual y la estación del año en un display LCD.

#include <Servo.h>
#include <IRremote.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
Servo myServo;

#define BOTON_APAGADO_PRENDIDO 0xFF00BF00
#define LED_ROJO 12
#define IR 11
#define LED_VERDE 10
#define SENSOR_TEMPERATURA A0

bool sistemaPrendido = false;
bool ledRojoPrendido = false;
bool ledVerdePrendido = false;
int contadorSistema = 0;
const char* mensaje = "";

void setup()
{
    Serial.begin(9600);
    IrReceiver.begin(IR, DISABLE_LED_FEEDBACK); 
    lcd.begin(16,2);
    pinMode(LED_ROJO, OUTPUT);
    pinMode(LED_VERDE, OUTPUT);
    myServo.attach(9,500,2500);
}

void loop()
{
    bool sistemaPrendido = detectarSistemaPrendidoApagado();

    if(sistemaPrendido)
    {
        int temperatura = leerTemperaturaGrados(SENSOR_TEMPERATURA);
        detectarEstacionIncendioMostrarLCD(temperatura);
        prenderApagarLedsServoIncendio(temperatura);
    }
    else
    {
        lcd.clear();
        lcd.noCursor();
        myServo.write(0);
        apagarLed(LED_ROJO);
        apagarLed(LED_VERDE);
    }
} 

void prenderApagarLedsServoIncendio(int temperatura)
{
    if(temperatura >= 60)
    {
        prenderLed(LED_ROJO);
        apagarLed(LED_VERDE);
        myServo.write(180);
        delay(500);
        myServo.write(0);
        delay(500);
    }
    else
    {
        prenderLed(LED_VERDE);
        apagarLed(LED_ROJO);
        myServo.write(0);
    }
}
void prenderLed(int led)
{
    switch(led)
    {
        case LED_ROJO:
            if(ledRojoPrendido == false)
            {
                Serial.println("Led Rojo prendido");
                ledRojoPrendido = true;
            }
        break;
        case LED_VERDE:
            if(ledVerdePrendido == false)
            {
                Serial.println("Led Verde prendido");
                ledVerdePrendido = true;
            }
        break;
    }
    
    digitalWrite(led, HIGH);  
}

void apagarLed(int led)
{
    switch(led)
    {
        case LED_ROJO:
            if(ledRojoPrendido == true)
            {
                Serial.println("Led Rojo apagado");
                ledRojoPrendido = false;
            }
        break;
        case LED_VERDE:
            if(ledVerdePrendido == true)
            {
                Serial.println("Led Verde apagado");
                ledVerdePrendido = false;
            }
        break;
    }
    
    digitalWrite(led, LOW);
}

int leerTemperaturaGrados(int sensor)
{
    int lectorSensorTemperatura = analogRead(sensor);
    int gradosSensorTemperatura = map(lectorSensorTemperatura, 20, 358, -40, 125);
    
    return gradosSensorTemperatura;
}

void detectarEstacionIncendioMostrarLCD(int temperatura)
{ 
    if(temperatura >= -40 && temperatura <= 5)
    {
        mensaje = "Estacion: Invierno";
    }
    else if(temperatura > 5 && temperatura <= 20)
    {
        mensaje = "Estacion: Otonio";
    }
    else if(temperatura > 20 && temperatura <= 30)
    {
        mensaje = "Estacion: Primavera";
    }
    else if(temperatura > 30 && temperatura < 60)
    {
        mensaje = "Estacion: Verano";
    }
    else
    {
        mensaje = "ALERTA: INCENDIO";
    }
    
    lcd.setCursor(0, 0);
    lcd.print("Temperatura: ");
    lcd.print(temperatura);
    lcd.print("   ");
    lcd.setCursor(0, 1);
    lcd.print(mensaje);
}

bool detectarSistemaPrendidoApagado()
{
    if(IrReceiver.decode())
    {
        if(IrReceiver.decodedIRData.decodedRawData == BOTON_APAGADO_PRENDIDO)
        {
            contadorSistema += 1;
            if(contadorSistema % 2 == 0)
            {
                sistemaPrendido = false;
                Serial.println("Sistema apagado");
            }
            else
            {
                sistemaPrendido = true;
                Serial.println("Sistema prendido");
            }
        }
        
        IrReceiver.resume();
    }
    delay(50);

    return sistemaPrendido;
}