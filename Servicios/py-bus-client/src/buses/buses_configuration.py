from pydantic import BaseModel, Field
from typing import Dict, List
import yaml


class BusesInfo(BaseModel):
    """Data needed for the buses defined"""

    num_stop: int = Field(default = None)
    lines: List[str] = Field(default = None)

class BusesList(BaseModel):
    """List of buses defined and its user"""

    bus_list: List[BusesInfo]

    @classmethod
    def read(cls, file: str):
        """
        Read buses from yaml file
        
        :param file: File to read configuration from
        :type file: str

        :return: List with the buses defined
        :rtype: BusesList
        """

        ret: List[BusesInfo] = []

        with open(file, 'r', encoding='utf-8') as f:

            tmp = yaml.safe_load(f)

            for t in tmp.get('buses', []):
                ret.append(BusesInfo(num_stop = t.get('num_stop', None),
                                     lines = t.get('lines', None)))

        return BusesList(bus_list = ret)