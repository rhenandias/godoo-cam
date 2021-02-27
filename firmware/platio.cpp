#include "platio.h"

PlatIO::PlatIO() {}

// Inicializa a biblioteca
void PlatIO::begin(Client &client, const char serverName[], const int serverPort, bool mode)
{
  this->client = &client;               // Wifi Cient, inicializado na sketch
  this->mode = mode;                    // Modo de operação (true para local, false para online)
  this->serverPort = serverPort;        // Porta para conexão ao servidor

  strcpy(this->serverName, serverName); // Nome do servidor para conexão

  // Monta o inínio da url de API separado para os modos LOCAL e ONLINE
  if (this->mode)
  {
    // Local Mode
    // http://192.168.x.xx:xxxx
    strcat(this->targetUrl, "http://");
    strcat(this->targetUrl, this->serverName);
    strcat(this->targetUrl, ":");
    char buf[5];
    itoa(this->serverPort, buf, 10);
    strcat(this->targetUrl, buf);
  }
  else
  {
    // Online Mode
    // https://domain.com
    strcat(this->targetUrl, "http://");
    strcat(this->targetUrl, this->serverName);
  }
}

// Realiza upload de um arquivo (por enquanto configurado para foto apenas)
int PlatIO::upload(const char path[], uint8_t *buf, size_t len)
{
  // O tamanho alocado deve ser capaz de conter a url completa de disparo da requisição url
  char url[120] = {0};

  strcat(url, this->targetUrl);
  strcat(url, path);

  Serial.println(F("URL de disparo da API de envio:"));
  Serial.println(url);

  Serial.print(F("Connecting to server: "));
  Serial.println(this->serverName);
  Serial.print(F("Connecting to port: "));
  Serial.println(this->serverPort);

  if (this->client->connect(this->serverName, this->serverPort))
  {
    Serial.println(F("Connection successful!"));

    String head = "--EspCamIOT\r\nContent-Disposition: form-data; name=\"foto\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--EspCamIOT--\r\n";

    uint16_t imageLen = len;
    uint16_t extraLen = head.length() + tail.length();
    uint16_t totalLen = imageLen + extraLen;

    this->client->println("POST " + String(url) + " HTTP/1.1");
    this->client->println("Host: " + String(this->serverName));
    this->client->println("Content-Length: " + String(totalLen));
    this->client->println("Content-Type:  multipart/form-data; boundary=EspCamIOT");
    this->client->println();
    this->client->print(head);

    uint8_t *fbBuf = buf;
    size_t fbLen = len;
    for (size_t n = 0; n < fbLen; n = n + 1024)
    {
      if (n + 1024 < fbLen)
      {
        this->client->write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen % 1024 > 0)
      {
        size_t remainder = fbLen % 1024;
        this->client->write(fbBuf, remainder);
      }
    }

    this->client->print(tail);
  }
  else
  {
    Serial.println(F("Connection Failure!"));
  }
  return 0;
}

// Método POST genérico em uma url
int PlatIO::postRaw(const char *url)
{
  HTTPClient http;
  http.begin(url);

  const int httpCode = http.POST("");
  if (httpCode > 0)
  {
    // Por enquanto não queremos fazer nada com o payload de resposta
    // String payload = http.getString();
  }
  else
  {
    Serial.println("Error on HTTP request");
  }
  http.end();

  return httpCode;
}

// Adquire o valor de uma chave em um objeto JSON representado em uma string
String PlatIO::getJSONValueByKey(String textToSearch, String key)
{
  if (textToSearch.length() == 0)
  {
    return String("");
  }

  String searchPhrase = String("\"");
  searchPhrase.concat(key);
  searchPhrase.concat("\":\"");

  int fromPosition = textToSearch.indexOf(searchPhrase, 0);

  if (fromPosition == -1)
  {
    // return because there is no status or it's null
    return String("");
  }

  fromPosition = fromPosition + searchPhrase.length();

  int toPosition = textToSearch.indexOf("\"", fromPosition);

  if (toPosition == -1)
  {
    // return because there is no end quote
    return String("");
  }

  textToSearch.remove(toPosition);

  return textToSearch.substring(fromPosition);
}
