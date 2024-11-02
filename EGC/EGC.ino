#include <Adafruit_SSD1306.h>
#define OLED_Address 0x3C // Dirección I2C de la pantalla OLED
Adafruit_SSD1306 oled(128, 64); // Crear el objeto de la pantalla OLED con resolución 128x64

int a = 0;          // Posición horizontal actual para dibujar la gráfica
int lasta = 0;      // Última posición horizontal
int lastb = 0;      // Última posición vertical
int LastTime = 0;   // Último tiempo de latido detectado
int ThisTime;       // Tiempo actual
bool BPMTiming = false; // Variable para controlar el tiempo de BPM
bool BeatComplete = false; // Variable para verificar si el latido se ha completado
int BPM = 0;        // Valor de BPM (latidos por minuto)
#define UpperThreshold 560  // Umbral superior de detección de pulso
#define LowerThreshold 530  // Umbral inferior de detección de pulso

void setup() {
    oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address); // Inicializar pantalla OLED
    oled.clearDisplay(); // Limpiar pantalla
    oled.setTextSize(2); // Configurar tamaño de texto
}
 
void loop() {
    // Reiniciar pantalla si la posición 'a' excede el ancho de la pantalla
    if (a > 127) {
        oled.clearDisplay();
        a = 0;
        lasta = a;
    }
 
    ThisTime = millis(); // Tiempo actual en milisegundos
    int value = analogRead(0); // Leer valor del sensor en el pin A0
    oled.setTextColor(WHITE); // Configurar color de texto

    // Ajuste de la gráfica para que tenga más altura en la pantalla
    int b = 60 - (value / 10); // Ajuste divisor para aumentar altura
    b = constrain(b, 0, 63); // Limitar 'b' dentro del rango visible

    // Ajuste de 'b' para que la gráfica no se mueva hacia arriba
    int adjustment = 10; // Ajuste adicional para mantener posición
    b = b + adjustment; // Compensa el movimiento hacia arriba

    b = constrain(b, 0, 63); // Asegurarse que 'b' esté dentro del rango visible

    oled.writeLine(lasta, lastb, a, b, WHITE); // Dibujar la línea de la gráfica
    lastb = b; // Guardar posición actual como la última
    lasta = a;
 
    // Detección de latido cuando el valor supera el umbral superior
    if (value > UpperThreshold) {
        if (BeatComplete) {
            // Calcular BPM basado en el tiempo entre latidos
            BPM = ThisTime - LastTime;
            BPM = int(60 / (float(BPM) / 1000)); // Convertir a BPM
            BPMTiming = false;
            BeatComplete = false;
            tone(8, 1000, 250); // Emitir sonido cuando se detecta un latido
        }
        if (BPMTiming == false) {
            LastTime = millis(); // Guardar tiempo actual al iniciar el cálculo
            BPMTiming = true;
        }
    }
    
    // Verificar si el valor está bajo el umbral inferior y la medición está activa
    if ((value < LowerThreshold) & (BPMTiming))
        BeatComplete = true; // Señalar que el latido se ha completado
 
    // Mostrar el BPM en la pantalla OLED
    oled.writeFillRect(0, 50, 128, 16, BLACK); // Limpiar área de texto del BPM
    oled.setCursor(0, 50);
    oled.print("BPM:");
    oled.print(BPM);
 
    oled.display(); // Actualizar pantalla OLED
    a++; // Avanzar en la posición horizontal de la gráfica
}
