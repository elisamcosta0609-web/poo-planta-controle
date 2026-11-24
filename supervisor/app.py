"""Interface de supervisão da planta didática."""

import json
import os
from pathlib import Path

import pandas as pd
import streamlit as st


def main():
    dados = Path(os.environ.get("POO_DADOS", "dados"))
    caminho = dados / "telemetria.json"

    st.title("Supervisório da planta didática")
    st.caption(f"Diretório de integração: {dados}")

    try:
        with caminho.open("r", encoding="utf-8") as arquivo:
            telemetria = json.load(arquivo)

    except FileNotFoundError:
        st.warning("Arquivo de telemetria não encontrado.")
        return

    except json.JSONDecodeError:
        st.error("Arquivo de telemetria contém JSON inválido.")
        return

    except OSError as erro:
        st.error(f"Erro ao ler telemetria: {erro}")
        return

    if not isinstance(telemetria, dict):
        st.error("Formato de telemetria inválido.")
        return

    leituras = telemetria.get("leituras", [])

    if not isinstance(leituras, list):
        st.error("Formato das leituras inválido.")
        return

    st.subheader("Leituras dos sensores")

    tabela = pd.DataFrame(leituras)
    st.dataframe(tabela, use_container_width=True)

    st.subheader("Alarmes")
    alarmes = telemetria.get("alarmes", [])

    if alarmes:
        for alarme in alarmes:
            st.warning(str(alarme))
    else:
        st.success("Nenhum alarme registrado.")

    st.subheader("Informações da planta")
    st.write("Sessão:", telemetria.get("sessao", "Não informada"))
    st.write("Ciclo:", telemetria.get("ciclo", "Não informado"))

    st.subheader("Estado dos atuadores")
    st.json(telemetria.get("atuadores", {}))


if __name__ == "__main__":
    main()
