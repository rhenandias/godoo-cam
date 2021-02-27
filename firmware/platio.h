#ifndef PLAT_IO
#define PLAT_IO

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

class PlatIO {
  public:
    PlatIO();

    // Inicialização
    void begin(Client &client, const char serverName[], const int serverPort, bool mode);

    // Realiza upload
    int upload(const char path[], uint8_t *buf, size_t len);

  private:
    char serverName[120];
    char targetUrl[120];
    int serverPort = 0;
    bool mode;

    Client *client = NULL;

    int postRaw(const char *url);

    String getJSONValueByKey(String textToSearch, String key);
};

#endif