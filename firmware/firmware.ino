#include <Arduino.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "platio.h"

#define LOCAL false
#define button 2

//Configuração da rede WiFi
const char *ssid = "VIVO-70D4";
const char *password = "aNU3497933";

WiFiClient client;
PlatIO platio;
String response;

// Configuração do modelo de câmera (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_hex_color 32
#define RESET_GPIO_hex_color -1
#define XCLK_GPIO_hex_color 0
#define SIOD_GPIO_hex_color 26
#define SIOC_GPIO_hex_color 27
#define Y9_GPIO_hex_color 35
#define Y8_GPIO_hex_color 34
#define Y7_GPIO_hex_color 39
#define Y6_GPIO_hex_color 36
#define Y5_GPIO_hex_color 21
#define Y4_GPIO_hex_color 19
#define Y3_GPIO_hex_color 18
#define Y2_GPIO_hex_color 5
#define VSYNC_GPIO_hex_color 25
#define HREF_GPIO_hex_color 23
#define PCLK_GPIO_hex_color 22

void setup()
{
  Serial.begin(115200);

  pinMode(button, INPUT_PULLUP);

  // Configuração da câmetra
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_hex_color;
  config.pin_d1 = Y3_GPIO_hex_color;
  config.pin_d2 = Y4_GPIO_hex_color;
  config.pin_d3 = Y5_GPIO_hex_color;
  config.pin_d4 = Y6_GPIO_hex_color;
  config.pin_d5 = Y7_GPIO_hex_color;
  config.pin_d6 = Y8_GPIO_hex_color;
  config.pin_d7 = Y9_GPIO_hex_color;
  config.pin_xclk = XCLK_GPIO_hex_color;
  config.pin_pclk = PCLK_GPIO_hex_color;
  config.pin_vsync = VSYNC_GPIO_hex_color;
  config.pin_href = HREF_GPIO_hex_color;
  config.pin_sscb_sda = SIOD_GPIO_hex_color;
  config.pin_sscb_scl = SIOC_GPIO_hex_color;
  config.pin_pwdn = PWDN_GPIO_hex_color;
  config.pin_reset = RESET_GPIO_hex_color;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_SVGA;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // Conexão WiFi
  Serial.println(F("Conectando wifi"));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }
  Serial.println(F("\nConectado"));

  // Configuração da biblioteca PlatIO
  // platio.begin(client, api_address, port, mode)
  // Para operar em localhost -> mode = true
  // Para operar online -> mode = false
  if (LOCAL)
    platio.begin(client, "192.168.15.5", 4000, true);
  else
    platio.begin(client, "godoo-cam.herokuapp.com", 80, false);

  // Inicialização da câmera
  // Deve ser a última etapa da configuração da ESP para evitar conflitos de memória RAM
  // Esse processo vai alocar as configurações da camera, isso demanda um pouco de espaço livre
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // Aloca espaço para a string que receberá respostas da biblioteca
  // Com isso, evitamos desfragmentação de memória para alocar e realocar a string a cada nova resposta
  // Alocação básica de 8 bytes é mais do que o suficiente para esse exemplo
  response.reserve(8);
}

void sendPhoto()
{
  // Realiza captura de imagem
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();

  // Verifica integridade da captura
  if (!fb)
  {
    // Serial.println("Camera capture failed");
    Serial.println(F("Falha na captura de imagem"));
  }
  else
  {
    Serial.println(F("Sucesso na captura de imagem"));
    Serial.print(F("Tamanho da imagem (bytes): "));
    Serial.println(fb->len);

    // Realiza envio da imagem para a API através da biblioteca PlatIO

    // Envio realizado no formato: platio.upload(api_endpoint, photo_buffer, photo_lenght)
    // Teoricamente, basta alterar o endpoint para o mesmo da API utilizada
    // O endereço já foi configurado previamente na inicialização da biblioteca
    // Assim, não seria necessário realizar mais alterações na biblioteca em si
    // E se essa for a única função utilizada, é possível limpar a biblioteca PlatIO para apenas
    // manter a funcionalidade de envio de imagem, reduzindo drasticamente o tamanho da biblioteca
    platio.upload("/api/pushphoto?path=camera/live", fb->buf, fb->len);
  }

  Serial.println(F("Ciclo de captura completo"));

  // A biblioteca de camera da ESP precisa receber como retorno o ponteiro para o frame capturado
  if (fb)
  {
    esp_camera_fb_return(fb);
  }
}

void loop()
{
  // Funcionamento através de botão físico na ESP (simulando um sensor)
  if(!digitalRead(button))
  {
    delay(500);
    Serial.println(F("Disparo de sensor. Tirar foto agora"));
    sendPhoto();
    Serial.println(F("Ciclo de sensor completo"));
  }

  // delay(1000);
}
