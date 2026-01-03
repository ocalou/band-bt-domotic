from pydantic import BaseModel, Field
from typing import Dict
import yaml


class BandData(BaseModel):
    """Data needed for each band"""

    user: str = Field(default = None)
    rssi: float = Field(default = None)
    room: str = Field(default = None)

class Bands(BaseModel):
    """List of bands defined and its user"""

    bands_list: Dict[str, BandData]

    @classmethod
    def read(cls, file: str):
        """
        Read bands from yaml file
        
        :param file: File to read configuration from
        :type file: str

        :return: List with the bands defined
        :rtype: Bands
        """

        ret: Dict[str, str] = {}

        with open(file, 'r', encoding='utf-8') as f:

            tmp = yaml.safe_load(f)

            for t in tmp.get('bands', []):
                ret[t.get('mac', None)] = BandData(user = t.get('user', None))

        return Bands(bands_list = ret)
