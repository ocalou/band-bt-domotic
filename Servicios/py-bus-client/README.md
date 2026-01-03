# py-bus-client

Servicio que se comunica con la API de buses de Coruña y envia por MQTT los datos de los buses definidos para su consulta. El tiempo de espera extraido de la consulta se publica por MQTT a los topicos ```buses/[num_parada]/[linea_bus]```

## Ficheros

La estructura de ficheros es la siguiente:

```
.
├── devops
├── resources
└── src
    ├── buses
    ├── clients
    └── configuration
```

- **[devops](./devops/)**: directorio donde se encuentran los ficheros para el despliegue del servicio.
- **[resources](./resources/)**: directorio donde se encuentran los ficheros de configuración asi como los requisitos para el despliegue.
- **[src](./src/)**: directorio con el codigo fuente del servicio.

## Configuracion

Para la configuracion del servicio, existe un [fichero](./resources/config.yaml) donde se pueden definir los parametros.

Para la definicion de las pulseras y el usuario de cada una, existe un [fichero](./resources/buses.yaml) donde se define el numero de parada junto a la linea de bus a consultar en esa parada.

## Despliegue
Existe un fichero [dockerfile](./devops/dockerfile) para la creacion de la imagen del contenedor docker para el despliegue del servicio.

Existe un [Makefile](./Makefile) para la construccion de la imagen y la ejecucion del contenedor.

- ```make build```: construye la imagen del contenedor.
-  ```make run```: inicia el contenedor.
-  ```make deploy```: inicia el contenedor en segundo plano.