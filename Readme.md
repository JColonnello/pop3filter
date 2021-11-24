# POP3FILTER

## Autores

- JOAQUÍN ROMÁN COLONNELLO
- LUCAS SEBASTIAN GOMEZ
- LUCÍA TORRUSIO

## Objetivo

Objetivo: El objetivo del trabajo es implementar un servidor proxy para el protocolo POP3 (Post Office Protocol versión 3) [RFC1957] que pueda ser usado por Mail User Agents tales como Mozilla Thunderbird, Microsoft Outlook y Evolution para la recepción de correos electronicos.  El proxy proveerá al usuario algunos servicios extras que el servidor de origen POP3 no provee.El usuario deberá configurar su MUA para que se utilice el proxy.  No será necesario soportar STARTTLS, TLS, SSL.


## Compilación

Posicionarse en la carpeta del proyecto y compilar con el siguiente comando:

```bash
git submodule update --init
make clean
make all
````

## Ejecución

La ejecución se realiza con el siguiente comando:

```bash
./build/pop3filter 
```
Para el client desde la carpeta "Client". Escribir los comandos directamente:
```bash
./build/client
```