"""Repositório responsável pela leitura da telemetria da planta."""

import json
from pathlib import Path


class RepositorioTelemetria:
    """Carrega e valida os dados de telemetria."""

    def __init__(self, caminho):
        self.caminho = Path(caminho)

    def carregar(self):
        """Lê a telemetria e verifica sua estrutura básica."""
        with self.caminho.open("r", encoding="utf-8") as arquivo:
            telemetria = json.load(arquivo)

        if not isinstance(telemetria, dict):
            raise ValueError("Formato de telemetria inválido.")

        leituras = telemetria.get("leituras", [])

        if not isinstance(leituras, list):
            raise ValueError("Formato das leituras inválido.")

        return telemetria
