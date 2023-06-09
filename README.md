# my-ftp

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