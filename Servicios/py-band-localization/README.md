# py-band-localization
Servicio que se encarga de recibir los mensajes MQTT de los nodos, comparar la calidad de la señal de todos los nodos y estimar la ubicación del usuario.

## Ficheros

La estructura de ficheros es la siguiente:

```
.
├── devops
├── resources
└── src
    ├── clients
    ├── configuration
    └── schemas
```

- **[devops](./devops/)**: directorio donde se encuentran los ficheros para el despliegue del servicio.
- **[resources](./resources/)**: directorio donde se encuentran los ficheros de configuración asi como los requisitos para el despliegue.
- **[src](./src/)**: directorio con el codigo fuente del servicio.

## Configuracion

Para la configuracion del servicio, existe un [fichero](./resources/config.yaml) donde se pueden definir los parametros.

Para la definicion de las pulseras y el usuario de cada una, existe un [fichero](./resources/bands.yaml) donde se define la MAC de la pulsera y el usuario al que le pertenece.

## Despliegue
Existe un fichero [dockerfile](./devops/dockerfile) para la creacion de la imagen del contenedor docker para el despliegue del servicio.

Existe un [Makefile](./Makefile) para la construccion de la imagen y la ejecucion del contenedor.

- ```make build```: construye la imagen del contenedor.
-  ```make run```: inicia el contenedor.
-  ```make deploy```: inicia el contenedor en segundo plano.