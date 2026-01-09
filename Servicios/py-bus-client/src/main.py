import time
from typing import List

from clients import APIClient, MQTTClient, BusInfoResponse, Response, BusStopInfo, BusStop
from configuration import Configuration
from buses import BusesList

if __name__ == '__main__':

    api_client: APIClient = APIClient()
    config: Configuration = Configuration.read('resources/config.yaml')
    buses: BusesList = BusesList.read('resources/buses.yaml')

    mqtt_c: MQTTClient = MQTTClient(host = config.MQTT_PARAMS.host,
                                    port = config.MQTT_PARAMS.port)

    buses_req: Response = Response(buses = {})
    stop: int
    info: List[BusStopInfo] = api_client.get_bus_stop_info()
    response: BusStop
    while 1:
        
        for buses_stop in buses.bus_list:
            stop = buses_stop.num_stop
            buses_req.buses[info[stop].name] = []

            response = api_client.get_buses_in_stop(stop)

            for bus in buses_stop.lines:
                b = response.get(bus)
                if(b):
                    buses_req.buses[info[stop].name].append(BusInfoResponse(bus = bus, time = b[0].time))

                
        mqtt_c.publish(config.MQTT_PARAMS.topic, buses_req.model_dump_json())

        time.sleep(config.SERVICE_PARAMS.delay)
