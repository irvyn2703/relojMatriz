# 🚀 Proyecto ESP32

Este proyecto está diseñado para ejecutarse en una placa **ESP32** y conectarse a una red Wi-Fi para interactuar con una API o plataforma externa (como SinricPro, Firebase, etc.).

---

## 📁 Archivos importantes

### `src/main.cpp`

Contiene el código principal que configura la conexión Wi-Fi, inicializa componentes, y ejecuta la lógica del dispositivo.

### `src/config.h` (no incluido en el repositorio)

Este archivo contiene configuraciones sensibles y **no debe subirse al repositorio**.

---

## ⚙️ Configuración

Antes de compilar este proyecto, debes crear un archivo llamado `config.h` dentro de la carpeta `src/` con el siguiente contenido:

```cpp
#ifndef CONFIG_H
#define CONFIG_H

// Credenciales WiFi
#define ssid "TuNombreDeRedWiFi"
#define password "TuContrasenaWiFi"

// Identificador del dispositivo
#define DEVICE_ID "ESP32_01"

// Claves de autenticación (por ejemplo, para SinricPro)
#define API_KEY "tu_api_key"
#define APP_SECRET "tu_app_secret"

#endif
```
