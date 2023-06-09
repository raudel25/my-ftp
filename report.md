- [Anabel Benítez González] (https://github.com/anabel02)
- [Raudel Alejandro Gómez Molina] (https://github.com/raudel25)
- [Anthuán Montes de Oca] (https://github.com/Nuebe29)

Este proyecto tiene como objetivo implementar un pequeño servidor web que permita la transferencia de archivos desde un servidor a un cliente.
Al ejecutarlo se le puede especificar el puerto y el root del servidor, de lo contrario se configura el puerto 5000 y el root del sistema.
Además el servidor puede manejar múltiples clientes de forma concurrente.
Un cliente puede realizar las siguientes acciones:
* Listar los archivos del directorio actual, con sus respectivos nombre, tamaño y fecha de modificación, y ordenarlos por cualquiera de estos campos.
* Cambiar de directorio, tanto al directorio padre como a un directorio hijo.
* Descargar un archivo del servidor.

El protocolo de comunicación es el HTTP, por lo que el servidor recibe una solicitud HTTP y envía una respuesta HTTP.
Este protocolo se basa en el intercambio de mensajes entre el cliente y el servidor, donde cada mensaje tiene un formato
específico.

## Detalles de Implementación

El flujo de los datos comienza en el método `main` que se encarga de definir el puerto y el root del servidor, en caso de que hayan sido introducidos
valida que el puerto sea un entero positivo y que el directorio exista y tenga permisos de lectura,
en caso de que no se hayan introducido se configura el puerto 5000 y el root del sistema.
Luego llama al método `loop` de `main.c` que se encarga de inicializar el servidor en el puerto especificado y de la creación de los hilos, 
que manejan las peticiones a través del método `handle_client` de `server`.

#### Mensaje de solicitud

El mensaje de solicitud tiene el siguiente formato:

```
<method> <path> <version>
<headers>
```

Donde:

- `<method>`: es el método de la solicitud, solo aceptamos GET.
- `<path>`: es el path del directorio al que se desea acceder o del archivo que se desea descargar.
- `<version>`: es la versión del protocolo, solo aceptamos HTTP/1.1.
- `<headers>`: son los headers de la solicitud.

#### Manejo de la Solicitud
Cuando se recibe una solicitud en el método handle_client se analiza que el método sea GET, además se revisa que la url no esté vacía, 
en caso de que no se cumpla alguna de estas condiciones se envía una respuesta con código de estado `400 Bad Request`.
Si se cumple con las condiciones se analiza si el path es un archivo o un directorio, en caso de que sea un archivo se llama al método `send_file` de `server.c`,
en caso de que sea un directorio se llama al método `navigate` de `server.c`. En caso de que el path no exista se envía una respuesta con código de estado `404 Not Found`.

El mensaje de respuesta tiene el siguiente formato:

```
<version> <status> <status_message>
<headers>
<content>
```

Donde:

- `<version>`: es la versión del protocolo, solo enviamos HTTP/1.1.
- `<status>`: es el código de estado de la respuesta.
- `<status_message>`: es el mensaje de estado de la respuesta.
- `<headers>`: son los headers de la respuesta.
- `<content>`: es el contenido de la respuesta.

#### Ok

En caso de que la descarga del archivo tenga éxito se envía una respuesta Http con código de estado `200 OK`, con headers 
`Content-Type`, `Content-Length` y `Content-Disposition` y el contenido es el archivo que se desea descargar.

En caso de que se tenga éxito al cambiar de directorio se envía el contenido del directorio, a través de una respuesta Http,
que es construida en `render` de `render.c`, esta respuesta tiene código de estado `200 OK`,
un header `Content-Type` que es `html` y el contenido se arma dinámicamente tomando la plantilla que se encuentra en el archivo
`resources/index.html` e insertando la tabla con el contenido del directorio, se listan los archivos del directorio utilizando
la función `readdir` de `dirent.h`, y se arman las columnas de la tabla con sus respectivos nombre, tamaño y fecha de modificación.
Se puede ordenar por cualquiera de los campos mencionados simplemente haciendo clic sobre el nombre del campo, esta ordenación se realiza 
en el lado del cliente, con código Javascript que se encuentra en el mencionado `index.html`, por tanto no ocurre petición al servidor para la ordenación.
Además contamos con un botón para volver al directorio padre, que es simplemente una referencia al directorio padre del directorio actual.

#### Errores

El servidor maneja los siguientes errores:

- `400 Bad Request`: se envía cuando la solicitud del cliente no es válida, por ejemplo cuando el método no es `GET`.
- `403 Forbidden`: se envía cuando el archivo o directorio al que se desea acceder no tiene permisos de lectura.
- `404 Not Found`: se envía cuando el archivo o directorio al que se desea acceder no existe.
- `500 Internal Server Error`: se envía cuando ocurre un error interno en el servidor.

Estos códigos de error se envían sin headers ni contenido.

### Múltiples Clientes

El servidor puede manejar múltiples clientes de forma concurrente, para esto
se crean hilos que se encargan de manejar a cada petición, esto ocurre en el método `loop` de `main.c`.