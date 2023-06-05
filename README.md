# my-ftp

## Integrantes
- Anabel Benítez González C311
- Raudel Alejandro Gómez Molina C311
- Anthuán Montes de Oca Pons C311

## Descrición del Proyecto
Este proyecto tiene como objetivo implementar un pequeño servidor web que permita la transferencia de archivos desde un servidor a un cliente.
Al ejecutarlo se le puede especificar el puerto y el root del servidor, de lo contrario se configura el puerto 5000 y el root del sistema.
Además el servidor puede manejar múltiples clientes de forma concurrente. 
Un cliente puede realizar las siguientes acciones:
* Listar los archivos del directorio actual, con sus respectivos nombre, tamaño y fecha de modificación.
* Cambiar de directorio, tanto al directorio padre como a un directorio hijo.
* Descargar un archivo del servidor.

El protocolo de comunicación es el HTTP, por lo que el servidor recibe una solicitud HTTP y envía una respuesta HTTP.
Este protocolo se basa en el intercambio de mensajes entre el cliente y el servidor, donde cada mensaje tiene un formato
específico.

## Ejecutar el proyecto

### Dependencias

El proyecto se encuentra desarrollado en `c 17`, y depende de la biblioteca
<a href="https://curl.se/libcurl/">libcurl</a>.
Para instalarla debe seguir los siguientes pasos.

Si está utilizando Ubuntu o Debian, puede instalar la biblioteca libcurl con el siguiente comando en la terminal:

```
sudo apt-get install libcurl4-openssl-dev
```

Si está usando otra distribución de Linux puede buscarla en el repositorio de paquetes de dicha distribución.

### Ejecutar el Proyecto

Para ejecutarlo de contar en su pc con el compilador de `c`, y ejecutar en su
terminal el siguiente comando:

```
make
```

Adicionalmente si cuenta con `cmake` puede encontrar la configuración en el archivo
`CMakeLists.txt`.


### Configurando el puerto y el root del Server

Por defecto el servidor se configura el puerto `5000` y el `home` de su sistema
como root. Para cambiar estos valores, debe especificarlos cuando ejecuta el
proyecto como se muestra a continuación:

```
make port=<port> root=<root>
```

Debe remplazar los valores `<port>` y `<root>` por el puerto y el root donde
desea configurar el server.

Otra forma de hacer configurar estos dos valores es crear el ejecutable con el
siguiente comando:

```
make build
```

y después ejecutar el ejecutable creado, especificando los valores:

```
./build/my_ftp <port> <root>
```

## Detalles de Implementación

El flujo de los datos comienza en el método `main` que se encarga de definir el puerto y el root del servidor, en caso de que hayan sido introducidos 
valida que el puerto sea un entero positivo y que el directorio exista y tenga permisos de lectura, 
en caso de que no se hayan introducido se configura el puerto 5000 y el root del sistema.
Luego llama al método `loop` de `main.c` que se encarga de inicializar el servidor en el puerto especificado y de la creación de los hilos
que manejan a los clientes.

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

#### Manejo de Ok

Si damos clic en el nombre de un archivo se descarga el archivo, esto ocurre en el servidor en el método `send_file` de `server.c`,
si se tiene éxito se envía una respuesta con código de estado `200 OK`, con headers `Content-Type`, `Content-Length` y `Content-Disposition` y el
contenido es el archivo que se desea descargar.

Si damos clic en el nombre de un directorio se cambia de directorio, esto ocurre en el servidor en el método `navigate` de `server.c`,
en caso de que se tenga éxito se envía una respuesta, que es construida en `render` de `render.c`, esta respuesta tiene código de estado `200 OK`, 
un header `Content-Type` que es `html` y el contenido se arma dinámicamente tomando la plantilla que se encuentra en el archivo 
`resources/index.html` e insertando la tabla con el contenido del directorio, se listan los archivos del directorio utilizando 
la función `readdir` de `dirent.h`, y se arman las columnas de la tabla con sus respectivos nombre, tamaño y fecha de modificación.
Se puede ordenar por cualquiera de los campos mencionados simplemente dando clic al nombre del campo, esta ordenación se realiza en el lado del cliente,
con código Javascript en el `index.html`, por tanto no ocurre petición al servidor.
Además contamos con un botón para volver al directorio padre, que es simplemente una referencia al directorio padre del directorio actual.

###  Manejo de Errores

El servidor maneja los siguientes errores:

- `400 Bad Request`: se envía cuando la solicitud del cliente no es válida, por ejemplo cuando el método no es `GET`.
- `404 Not Found`: se envía cuando el archivo o directorio al que se desea acceder no existe.
- `403 Forbidden`: se envía cuando el archivo o directorio al que se desea acceder no tiene permisos de lectura.
- `500 Internal Server Error`: se envía cuando ocurre un error interno en el servidor.

Estos códigos de error se envían sin headers ni contenido.

### Manejo de Clientes

El servidor puede manejar múltiples clientes de forma concurrente, para esto
se crean hilos que se encargan de manejar a cada cliente.