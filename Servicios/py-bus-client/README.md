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

## Configuración

Para la configuracion del servicio, existe un [fichero](./resources/config.yaml) donde se pueden definir los parametros.

Para la configuración de **mqtt:**
- **host:** ip del broker MQTT.
- **port:** puerto del broker MQTT.

Para la configuración del servicio (service):
- **delay:** tiempo entre consultas de los buses en segundos.

Para la definicion de las pulseras y el usuario de cada una, existe un [fichero](./resources/buses.yaml) donde se define el numero de parada junto a la linea de bus a consultar en esa parada.

**buses** es una lista de las paradas a consultar:
- **num_stop**: número de la parada a consultar (en entero).
- **lines**: lista de las líneas de esa parada a consultar (como string).

Por ejemplo:

```yaml
buses:
    - num_stop: 263
      lines:
        - '6A'
        - '7'
    - num_stop: 160
      lines:
        - '3'
        - '3A'
```


## Despliegue
Existe un fichero [dockerfile](./devops/dockerfile) para la creacion de la imagen del contenedor docker para el despliegue del servicio.

Existe un [Makefile](./Makefile) para la construccion de la imagen y la ejecucion del contenedor.

- ```make build```: construye la imagen del contenedor.
-  ```make run```: inicia el contenedor.
-  ```make deploy```: inicia el contenedor en segundo plano.

## Funcionamiento
Cuando el servicio se está ejecutando, cada x segundos (definidos en la configuración) se publica un json en el tópico ```buses```como:

```json
{
    "buses":
        {
            "Av. de Hércules, 115":
            [
                {"bus":"6A","time":"50"},
                {"bus":"7","time":"15"}
            ],
            "Juan Flórez, 10":
            [
                {"bus":"3","time":"67"},
                {"bus":"3A","time":"25"}
            ]
        }
}

```