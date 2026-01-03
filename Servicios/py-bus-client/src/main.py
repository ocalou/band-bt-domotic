import time

from clients import APIClient, MQTTClient
from configuration import Configuration
from buses import BusesList

if __name__ == '__main__':

    api_client: APIClient = APIClient()
    config: Configuration = Configuration.read('resources/config.yaml')
    buses: BusesList = BusesList.read('resources/buses.yaml')

    mqtt_c: MQTTClient = MQTTClient(host = config.MQTT_PARAMS.host,
                                    port = config.MQTT_PARAMS.port)
    
    bus_stop: int = buses.bus_list[0].num_stop
    bus: str | int = buses.bus_list[0].lines[0]

    while 1:
        
        b = api_client.get_buses_in_stop(bus_stop).get(bus)

        mqtt_c.publish(f'bus/{bus_stop}/{bus}', b[0].time)

        time.sleep(config.SERVICE_PARAMS.delay)
