# my-ftp

Este proyecto tiene como objetivo la creación de un `ftp` para `Linux` en `c`.

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
./build <port> <root>
```
