"""API Bus Client"""

from requests import get, Response
import json
from typing import Dict

from .schemas import AllBuses, BusLine, BusStop


class APIClient:

    _url = 'https://itranvias.com/queryitr_v3.php?'

    def __init__(self) -> "APIClient":
        """
        Constructor for APIClient Class

        :return APIClient
        """
        
        self._bus_lines: AllBuses = self.get_all_buses()

    def get_buses_in_stop(self,
                          stop: int) -> BusStop:
        """
        Get current buses in specific bus stop
        
        :param int stop: Bus stop ID number

        :return BusStop: Information of Buses available in Bus stop
        """

        _func = 0

        resp = self._generic_request(func = _func,
                                    data = stop)
        
        resp = json.loads(resp.content)

        buses: BusStop = BusStop(**(resp.get('buses', {})))

        ret: Dict = {}

        for b in buses.lines:
            ret[self._bus_lines[b.line].line_number] = b.buses

        return ret
        
    def get_all_buses(self) -> AllBuses:
        """
        Get all buses information
        
        :return AllBuses: List of all available buses
        """

        resp = self._generic_request(func = 7,
                              data = '20160101T000000_gl_0_20160101T000000')
        
        resp = json.loads(resp.content)

        bus_lines = AllBuses(**resp['iTranvias']['actualizacion'])

        self._bus_lines = self._parse_all_buses(bus_lines.bus_lines)

        # Fix due to API not including line in general request 
        self._bus_lines[2452] = BusLine(line_number = 'UDC',
                                        origin = 'Campus de Elviña',
                                        destination = 'UDC')
        
        return self._bus_lines
    
    def _parse_all_buses(self,
                           bus_lines: AllBuses) -> Dict[int, BusLine]:
        """
        Convert Buses received from API to connect with Bus Line name

        :param AllBuses bus_lines: List of all buses

        :return Dict[int, BusLine]: Dictionary with code of bus as key and Bus Line info as value
        """
        
        ret: Dict = {}

        for b in bus_lines:
            ret[b.id] = BusLine(**b.model_dump())

        return ret

    def _generic_request(self,
                        func: int,
                        data: str | int) -> Response:
        """
        Send Request to API

        :param int func: Function to send to the API
        :param str | int data: Data to send to the API

        :return Response: response from the API
        """
        
        req = f'{self._url}&dato={data}&func={func}'

        return get(req)