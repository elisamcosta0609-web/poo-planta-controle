"""Testes autorais do repositório de telemetria."""

import json
from pathlib import Path
import tempfile
import unittest

from supervisor.repositorio_telemetria import RepositorioTelemetria


class TestRepositorioTelemetria(unittest.TestCase):

    def test_carrega_telemetria_valida(self):
        with tempfile.TemporaryDirectory() as diretorio:
            caminho = Path(diretorio) / "telemetria.json"

            dados = {
                "sessao": "teste",
                "ciclo": 1,
                "leituras": [{"tag": "LT-01", "valor": 63.25}]
            }

            caminho.write_text(json.dumps(dados), encoding="utf-8")

            repositorio = RepositorioTelemetria(caminho)
            resultado = repositorio.carregar()

            self.assertEqual(resultado["sessao"], "teste")
            self.assertEqual(resultado["ciclo"], 1)
            self.assertEqual(resultado["leituras"][0]["valor"], 63.25)

    def test_rejeita_telemetria_que_nao_e_dicionario(self):
        with tempfile.TemporaryDirectory() as diretorio:
            caminho = Path(diretorio) / "telemetria.json"
            caminho.write_text(json.dumps([]), encoding="utf-8")

            repositorio = RepositorioTelemetria(caminho)

            with self.assertRaises(ValueError):
                repositorio.carregar()

    def test_rejeita_leituras_que_nao_sao_lista(self):
        with tempfile.TemporaryDirectory() as diretorio:
            caminho = Path(diretorio) / "telemetria.json"
            dados = {"leituras": "invalido"}
            caminho.write_text(json.dumps(dados), encoding="utf-8")

            repositorio = RepositorioTelemetria(caminho)

            with self.assertRaises(ValueError):
                repositorio.carregar()

    def test_rejeita_json_invalido(self):
        with tempfile.TemporaryDirectory() as diretorio:
            caminho = Path(diretorio) / "telemetria.json"
            caminho.write_text("{", encoding="utf-8")

            repositorio = RepositorioTelemetria(caminho)

            with self.assertRaises(json.JSONDecodeError):
                repositorio.carregar()

    def test_identifica_arquivo_inexistente(self):
        with tempfile.TemporaryDirectory() as diretorio:
            caminho = Path(diretorio) / "inexistente.json"
            repositorio = RepositorioTelemetria(caminho)

            with self.assertRaises(FileNotFoundError):
                repositorio.carregar()


if __name__ == "__main__":
    unittest.main()
