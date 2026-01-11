from pydantic import BaseModel
from typing import List, Optional, Dict
import yaml


class Condition(BaseModel):
    """Condition definition"""

    value: str | int
    operator: str
    topic: Optional[str] = None

class Action(BaseModel):
    """Action definition"""

    topic: str
    payload: str
    retain: bool

class Automation(BaseModel):
    """Automation definition"""

    name: str
    trigger_topic: str
    conditions: List[Condition]
    action: Action

class Automations(BaseModel):
    """Automations list"""

    automations: Dict[str, Automation] = {}

    @classmethod
    def read(cls, file: str):

        ret: Dict[str, Automation] = {}

        with open(file, 'r', encoding='utf-8') as f:

            tmp = yaml.safe_load(f)

            for t in tmp.get('automations', []):
                ret[t.get('trigger_topic', None)] = Automation(**t)

        return Automations(automations = ret)
